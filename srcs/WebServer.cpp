/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 01:19:56 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/12 19:02:08 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"

#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <csignal>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>

#include "CgiHandler.hpp"
#include "Handlers.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Router.hpp"
#include "WebServerException.hpp"

WebServer::WebServer(std::vector<Server> const& servers) : _servers(servers)
{
    signal(SIGCHLD, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    for (size_t i = 0; i < _servers.size(); ++i)
    {
        struct addrinfo hints;
        std::memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_NUMERICHOST;

        std::ostringstream portStr;
        portStr << _servers[i].port;

        struct addrinfo* res = NULL;
        int gaiErr = getaddrinfo(
            _servers[i].host.c_str(), portStr.str().c_str(), &hints, &res);
        if (gaiErr != 0)
        {
            _closeSockets();
            throw WebServerException(
                "getaddrinfo", _servers[i].host, _servers[i].port, gaiErr);
        }

        int fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (fd == -1)
        {
            freeaddrinfo(res);
            _closeSockets();
            throw WebServerException(
                "socket", _servers[i].host, _servers[i].port);
        }

        int opt = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        {
            close(fd);
            freeaddrinfo(res);
            _closeSockets();
            throw WebServerException(
                "setsockopt", _servers[i].host, _servers[i].port);
        }

        if (bind(fd, res->ai_addr, res->ai_addrlen) == -1)
        {
            close(fd);
            freeaddrinfo(res);
            _closeSockets();
            throw WebServerException(
                "bind", _servers[i].host, _servers[i].port);
        }

        freeaddrinfo(res);

        if (listen(fd, SOMAXCONN) == -1)
        {
            close(fd);
            _closeSockets();
            throw WebServerException(
                "listen", _servers[i].host, _servers[i].port);
        }

        if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
        {
            close(fd);
            _closeSockets();
            throw WebServerException(
                "fcntl", _servers[i].host, _servers[i].port);
        }

        _sockets.push_back(fd);
    }
}

WebServer::~WebServer()
{
    _closeSockets();
    _closeClients();
}

void WebServer::run()
{
    _initPollFds();

    while (true)
    {
        if (poll(&_pollfds[0], _pollfds.size(), POLL_TIMEOUT_MS) == -1)
        {
            if (errno == EINTR)
                continue;
            throw WebServerException("poll() failed");
        }

        for (size_t i = 0; i < _pollfds.size(); ++i)
        {
            int fd = _pollfds[i].fd;
            short revents = _pollfds[i].revents;
            size_t prev_size = _pollfds.size();

            if (revents & (POLLERR | POLLNVAL))
            {
                if (_pipe_to_client.count(fd))
                    _removeCgiPipe(fd);
                else if (!_isListeningSocket(fd))
                    _removeClient(fd);
            }
            else if (revents & (POLLIN | POLLHUP))
            {
                if (_isListeningSocket(fd))
                    _acceptClient(fd);
                else if (_pipe_to_client.count(fd))
                    _readCgiOutput(fd);
                else
                    _readClient(fd);
            }
            else if (revents & POLLOUT)
            {
                if (_pipe_to_client.count(fd))
                    _writeCgiBody(fd);
                else
                    _sendResponse(fd);
            }

            if (_pollfds.size() < prev_size)
                --i;
        }

        _checkCgiTimeouts();
    }
}

std::vector<Server> const& WebServer::getServers() const
{
    return _servers;
}

std::vector<int> const& WebServer::getSockets() const
{
    return _sockets;
}

void WebServer::_initPollFds()
{
    for (size_t i = 0; i < _sockets.size(); ++i)
        _addPollFd(_sockets[i], POLLIN);
}

void WebServer::_closeSockets()
{
    for (size_t i = 0; i < _sockets.size(); ++i)
        close(_sockets[i]);
    _sockets.clear();
}

int WebServer::_findPollFd(int fd) const
{
    for (int i = 0; i < (int)_pollfds.size(); ++i)
        if (_pollfds[i].fd == fd)
            return i;
    return -1;
}

void WebServer::_addPollFd(int fd, short events)
{
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = events;
    pfd.revents = 0;
    _pollfds.push_back(pfd);
}

void WebServer::_setPollEvent(int fd, short events)
{
    int i = _findPollFd(fd);
    if (i != -1)
        _pollfds[i].events = events;
}

void WebServer::_removeClient(int client_fd)
{
    Client* client = _clients[client_fd];

    if (client->hasCgi())
    {
        CgiProcess const& proc = client->getCgi();
        if (_pipe_to_client.count(proc.pipe_in))
            _removeCgiPipe(proc.pipe_in);
        if (_pipe_to_client.count(proc.pipe_out))
            _removeCgiPipe(proc.pipe_out);
    }

    close(client_fd);
    delete client;
    _clients.erase(client_fd);
    int i = _findPollFd(client_fd);
    if (i != -1)
        _pollfds.erase(_pollfds.begin() + i);
}

void WebServer::_closeClients()
{
    std::map<int, Client*>::iterator it;
    for (it = _clients.begin(); it != _clients.end(); ++it)
    {
        close(it->first);
        delete it->second;
    }
    _clients.clear();
}

bool WebServer::_isListeningSocket(int listening_fd) const
{
    for (size_t i = 0; i < _sockets.size(); ++i)
        if (_sockets[i] == listening_fd)
            return true;
    return false;
}

static std::string ipToString(struct in_addr addr)
{
    unsigned long hostOrder = ntohl(addr.s_addr);
    std::ostringstream oss;
    oss << ((hostOrder >> 24) & 0xFF) << "." << ((hostOrder >> 16) & 0xFF)
        << "." << ((hostOrder >> 8) & 0xFF) << "." << (hostOrder & 0xFF);
    return oss.str();
}

void WebServer::_acceptClient(int listening_fd)
{
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    int client_fd = accept(listening_fd, (struct sockaddr*)&client_addr, &len);
    if (client_fd == -1)
    {
        std::cerr << "[error] accept() failed: " << strerror(errno)
                  << std::endl;
        return;
    }

    Client* client = new Client(client_fd,
                                ipToString(client_addr.sin_addr),
                                ntohs(client_addr.sin_port));
    _clients[client_fd] = client;
    std::cout << *client << " has been accepted" << std::endl;

    if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1)
    {
        std::cerr << "[error] fcntl() failed: " << strerror(errno) << std::endl;
        _removeClient(client_fd);
        return;
    }

    _addPollFd(client_fd, POLLIN);
}

void WebServer::_readClient(int client_fd)
{
    char buf[4096];

    ssize_t n = recv(client_fd, buf, sizeof(buf), 0);

    if (n <= 0)
    {
        if (n == 0)
            std::cout << "[info] client disconnected: "
                      << _clients[client_fd]->getIp() << std::endl;
        else
            std::cerr << "[error] " << _clients[client_fd]->getIp()
                      << " recv failed: " << strerror(errno) << std::endl;
        _removeClient(client_fd);
        return;
    }

    Client* client = _clients[client_fd];
    client->appendRequest(buf, n);

    HttpRequest const& request = client->getRequest();
    if (!request.isComplete() && !request.hasError())
    {
        Server const& server =
            findServer(_servers, request.getHeader("host"), client_fd);
        client->checkBodyLimit(
            static_cast<size_t>(server.client_max_body_size));
    }

    if (request.isComplete() || request.hasError())
        _handleRequest(client_fd);
}

void WebServer::_handleRequest(int client_fd)
{
    Client* client = _clients[client_fd];
    HttpRequest const& request = client->getRequest();

    std::cout << "[fd=" << client_fd << "] request complete" << std::endl;
    // std::cout << client->getRequest() << std::endl;

    HttpResponse response;
    if (request.hasError())
    {
        std::cerr << "[error] " << _clients[client_fd]->getIp()
                  << " request failed: " << request.getError() << std::endl;
        response.setStatus(request.getErrorStatus());
        response.buildDefaultErrorBody();
        client->setResponse(response.build());
        _setPollEvent(client_fd, POLLOUT);
        return;
    }

    Server const& server =
        findServer(_servers, request.getHeader("host"), client_fd);

    response.setVersion(request.getVersion());

    if (_isValidRequest(request, server, response))
        _handleMethod(client_fd, request, server, response);

    if (!client->hasCgi())
        _finalizeResponse(client_fd, response, server);
}

bool WebServer::_isValidRequest(HttpRequest const& request,
                                Server const& server,
                                HttpResponse& response)
{
    if (request.getBody().size() >
        static_cast<size_t>(server.client_max_body_size))
    {
        response.setStatus(413);
        return false;
    }
    return true;
}

void WebServer::_handleMethod(int client_fd,
                              HttpRequest const& request,
                              Server const& server,
                              HttpResponse& response)
{
    std::string const path = request.getPath();
    Location const* loc = findLocation(path, server);

    if (loc == NULL)
    {
        response.setStatus(404);
        return;
    }

    if (!loc->redirect.empty())
    {
        response.setStatus(301);
        response.setHeader("Location", loc->redirect);
        return;
    }

    std::string const method = request.getMethod();

    if (loc->methods.count(method) == 0)
    {
        response.setStatus(405);
        return;
    }

    if (isCgi(request, *loc))
        _handleCgi(client_fd, request, *loc, response);
    else if (method == "GET")
        handleGet(path, *loc, response);
    else if (method == "POST")
    {
        std::string body = request.getBody();
        handlePost(path, body, *loc, response);
    }
    else if (method == "DELETE")
        handleDelete(path, *loc, response);
    else
        response.setStatus(405);
}

void WebServer::_handleError(HttpResponse& res,
                             Server const& server,
                             std::string const& client_ip)
{
    if (res.getStatus() < 400)
        return;

    std::map<int, std::string> const& errorPages = server.error_pages;
    std::map<int, std::string>::const_iterator it =
        errorPages.find(res.getStatus());

    if (it != errorPages.end())
    {
        std::ifstream file(it->second.c_str());
        if (file.is_open())
        {
            std::string content((std::istreambuf_iterator<char>(file)),
                                std::istreambuf_iterator<char>());
            res.setBody(content);
            res.setHeader("Content-Type", "text/html");
            return;
        }

        std::cerr << "[error] " << client_ip
                  << " cannot open error page: " << it->second << ": "
                  << strerror(errno) << std::endl;
    }
    res.buildDefaultErrorBody();
}

void WebServer::_finalizeResponse(int client_fd,
                                  HttpResponse& response,
                                  Server const& server)
{
    Client* client = _clients[client_fd];
    _handleError(response, server, client->getIp());
    client->setResponse(response.build());
    std::cout << "[fd=" << client_fd << "] response complete" << std::endl;
    // std::cout << client->getResponse() << std::endl;
    _setPollEvent(client_fd, POLLOUT);
}

void WebServer::_sendResponse(int client_fd)
{
    Client* client = _clients[client_fd];
    std::string const& response = client->getResponse();
    size_t sent = client->getSentBytes();

    ssize_t n =
        send(client_fd, response.c_str() + sent, response.size() - sent, 0);

    if (n <= 0)
    {
        if (n == 0)
            std::cout << "[info] client disconnected: " << client->getIp()
                      << std::endl;
        else
            std::cerr << "[error] " << client->getIp()
                      << " send failed: " << strerror(errno) << std::endl;
        _removeClient(client_fd);
        return;
    }

    client->addSentBytes(n);
    if (client->getSentBytes() >= response.size())
        _removeClient(client_fd);
}

void WebServer::_handleCgi(int client_fd,
                           HttpRequest const& request,
                           Location const& loc,
                           HttpResponse& response)
{
    CgiProcess proc = spawnCgi(request, loc);
    if (proc.pid == -1)
    {
        response.setStatus(500);
        return;
    }
    _clients[client_fd]->setCgi(proc);
    _addPollFd(proc.pipe_out, POLLIN);
    _pipe_to_client[proc.pipe_out] = client_fd;
    if (request.getMethod() == "POST")
    {
        _addPollFd(proc.pipe_in, POLLOUT);
        _pipe_to_client[proc.pipe_in] = client_fd;
    }
    else
        close(proc.pipe_in);
}

void WebServer::_removeCgiPipe(int cgi_fd)
{
    int i = _findPollFd(cgi_fd);
    if (i != -1)
        _pollfds.erase(_pollfds.begin() + i);
    _pipe_to_client.erase(cgi_fd);
    close(cgi_fd);
}

void WebServer::_readCgiOutput(int cgi_fd)
{
    int client_fd = _pipe_to_client[cgi_fd];
    Client* client = _clients[client_fd];
    char buf[4096];

    ssize_t n = read(cgi_fd, buf, sizeof(buf));

    if (n <= 0)
    {
        _removeCgiPipe(cgi_fd);

        HttpResponse response;
        response.setVersion(client->getRequest().getVersion());

        if (n == 0)
        {
            std::cout << "[info] cgi output complete: " << client->getIp()
                      << std::endl;
            buildCgiResponse(client->getCgiOutput(), response);
        }
        else
        {
            std::cerr << "[error] cgi read failed: " << client->getIp() << ": "
                      << strerror(errno) << std::endl;
            response.setStatus(502);
        }

        if (_pipe_to_client.count(client->getCgi().pipe_in))
            _removeCgiPipe(client->getCgi().pipe_in);

        Server const& server = findServer(
            _servers, client->getRequest().getHeader("host"), client_fd);
        client->clearCgi();
        _finalizeResponse(client_fd, response, server);

        return;
    }

    client->appendCgiOutput(buf, n);
}

void WebServer::_writeCgiBody(int cgi_fd)
{
    int client_fd = _pipe_to_client[cgi_fd];
    Client* client = _clients[client_fd];
    std::string const& body = client->getRequest().getBody();
    size_t sent = client->getCgi().body_sent;

    ssize_t n = write(cgi_fd, body.c_str() + sent, body.size() - sent);

    if (n <= 0)
    {
        if (n == 0)
            std::cout << "[info] cgi pipe closed: " << client->getIp()
                      << std::endl;
        else
            std::cerr << "[error] " << client->getIp()
                      << " cgi write failed: " << strerror(errno) << std::endl;
        _removeCgiPipe(cgi_fd);
        return;
    }

    client->addCgiBodySent(n);
    if (client->getCgi().body_sent >= body.size())
        _removeCgiPipe(cgi_fd);
}

void WebServer::_checkCgiTimeouts()
{
    time_t now = time(NULL);
    std::vector<int> timed_out;

    std::map<int, Client*>::iterator it;
    for (it = _clients.begin(); it != _clients.end(); ++it)
    {
        Client* client = it->second;
        if (client->hasCgi() &&
            now - client->getCgi().started > CGI_TIMEOUT_SECONDS)
            timed_out.push_back(it->first);
    }

    for (size_t i = 0; i < timed_out.size(); ++i)
        _killCgi(timed_out[i]);
}

void WebServer::_killCgi(int client_fd)
{
    Client* client = _clients[client_fd];
    CgiProcess const& proc = client->getCgi();

    std::cerr << "[error] " << client->getIp() << " cgi timed out, killing pid "
              << proc.pid << std::endl;
    kill(proc.pid, SIGKILL);

    if (_pipe_to_client.count(proc.pipe_in))
        _removeCgiPipe(proc.pipe_in);
    if (_pipe_to_client.count(proc.pipe_out))
        _removeCgiPipe(proc.pipe_out);

    HttpResponse response;
    response.setVersion(client->getRequest().getVersion());
    response.setStatus(504);

    Server const& server =
        findServer(_servers, client->getRequest().getHeader("host"), client_fd);
    client->clearCgi();
    _finalizeResponse(client_fd, response, server);
}

std::ostream& operator<<(std::ostream& os, WebServer const& ws)
{
    std::vector<Server> const& servers = ws.getServers();
    std::vector<int> const& sockets = ws.getSockets();

    for (size_t i = 0; i < servers.size(); ++i)
    {
        Server const& s = servers[i];

        os << "[server " << i << "] " << s.host << ":" << s.port
           << " (fd=" << sockets[i] << ")\n";

        os << "  server_name:          "
           << (s.server_name.empty() ? "-" : s.server_name) << "\n";
        os << "  root:                 " << (s.root.empty() ? "-" : s.root)
           << "\n";
        os << "  client_max_body_size: " << s.client_max_body_size << "\n";

        os << "  error_pages:\n";
        std::map<int, std::string>::const_iterator it;
        for (it = s.error_pages.begin(); it != s.error_pages.end(); ++it)
            os << "    " << it->first << " -> " << it->second << "\n";

        os << "  locations:\n";
        for (size_t j = 0; j < s.locations.size(); ++j)
        {
            Location const& loc = s.locations[j];
            os << "    [" << loc.path << "]\n";
            os << "      methods:    ";
            std::set<std::string>::const_iterator mit;
            for (mit = loc.methods.begin(); mit != loc.methods.end(); ++mit)
                os << *mit << " ";
            os << "\n";
            os << "      root:       " << loc.root << "\n";
            os << "      index:      " << loc.index << "\n";
            os << "      autoindex:  " << (loc.autoindex ? "on" : "off")
               << "\n";
            os << "      upload:     "
               << (loc.upload_path.empty() ? "-" : loc.upload_path) << "\n";
            os << "      redirect:   "
               << (loc.redirect.empty() ? "-" : loc.redirect) << "\n";
            os << "      cgi:\n";
            std::map<std::string, std::string>::const_iterator cgi_it;
            for (cgi_it = loc.cgi.begin(); cgi_it != loc.cgi.end(); ++cgi_it)
                os << "        " << cgi_it->first << " -> " << cgi_it->second
                   << "\n";
        }
    }
    return os;
}
