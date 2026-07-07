/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 01:19:56 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/07 17:12:55 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>

#include "Handlers.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "WebServerException.hpp"

WebServer::WebServer(std::vector<Server> const& servers) : _servers(servers)
{
    for (size_t i = 0; i < _servers.size(); ++i)
    {
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd == -1)
        {
            _closeSockets();
            throw WebServerException(
                "socket", _servers[i].host, _servers[i].port);
        }

        struct sockaddr_in addr;
        std::memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(_servers[i].port);
        addr.sin_addr.s_addr = inet_addr(_servers[i].host.c_str());

        int opt = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        {
            close(fd);
            _closeSockets();
            throw WebServerException(
                "setsockopt", _servers[i].host, _servers[i].port);
        }

        if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
        {
            close(fd);
            _closeSockets();
            throw WebServerException(
                "bind", _servers[i].host, _servers[i].port);
        }

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
        if (poll(&_pollfds[0], _pollfds.size(), -1) == -1)
            throw WebServerException("poll() failed");

        for (size_t i = 0; i < _pollfds.size(); ++i)
        {
            if (_pollfds[i].revents & POLLIN)
            {
                if (_isListeningSocket(_pollfds[i].fd))
                    _acceptClient(_pollfds[i].fd);
                else
                {
                    size_t prev_size = _pollfds.size();
                    _readClient(_pollfds[i].fd);
                    if (_pollfds.size() < prev_size)
                        --i;
                }
            }
            else if (_pollfds[i].revents & POLLOUT)
            {
                size_t prev_size = _pollfds.size();
                _sendResponse(_pollfds[i].fd);
                if (_pollfds.size() < prev_size)
                    --i;
            }
        }
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
    close(client_fd);
    delete _clients[client_fd];
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

Server const& WebServer::_findServer(std::string const& host) const
{
    std::string hostname = host.substr(0, host.find(":"));
    for (size_t i = 0; i < _servers.size(); ++i)
        if (_servers[i].server_name == hostname)
            return _servers[i];
    return _servers[0];
}

bool WebServer::_isListeningSocket(int listening_fd) const
{
    for (size_t i = 0; i < _sockets.size(); ++i)
        if (_sockets[i] == listening_fd)
            return true;
    return false;
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
                                inet_ntoa(client_addr.sin_addr),
                                ntohs(client_addr.sin_port));
    _clients[client_fd] = client;
    std::cout << *client << " has been accepted \n" << std::endl;

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

    if (client->getRequest().isComplete())
        _handleRequest(client_fd);
}

void WebServer::_handleRequest(int client_fd)
{
    Client* client = _clients[client_fd];
    HttpRequest const& request = client->getRequest();

    std::cout << "[fd=" << client_fd << "] request complete:\n";
    std::cout << client->getRequest() << std::endl;

    HttpResponse response;
    if (request.hasError())
    {
        std::cerr << "[error] " << _clients[client_fd]->getIp()
                  << " request failed: " << request.getError() << std::endl;
        response.setStatus(400);
        response.buildDefaultErrorBody();
        client->setResponse(response.build());
        _setPollEvent(client_fd, POLLOUT);
        return;
    }

    Server const& server = _findServer(request.getHeader("host"));

    response.setVersion(request.getVersion());

    if (_isValidRequest(request, server, response))
        _handleMethod(request, server, response);

    _handleError(response, server, client->getIp());

    client->setResponse(response.build());

    std::cout << "[fd=" << client_fd << "] response complete:\n";
    std::cout << client->getResponse() << std::endl;
    _setPollEvent(client_fd, POLLOUT);
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

void WebServer::_handleMethod(HttpRequest const& request,
                              Server const& server,
                              HttpResponse& response)
{
    std::string method = request.getMethod();
    std::string uri = request.getUri();

    if (method == "GET")
        handleGet(uri, server, response);
    else if (method == "POST")
    {
        std::string body = request.getBody();
        handlePost(uri, body, server, response);
    }
    else if (method == "DELETE")
        handleDelete(uri, server, response);
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

void WebServer::_sendResponse(int client_fd)
{
    Client* client = _clients[client_fd];
    std::string const& response = client->getResponse();
    size_t sent = client->getSentBytes();

    ssize_t n =
        send(client_fd, response.c_str() + sent, response.size() - sent, 0);

    if (n == -1)
    {
        std::cerr << "[error] " << client->getIp()
                  << " send failed: " << strerror(errno) << std::endl;
        _removeClient(client_fd);
        return;
    }

    client->addSentBytes(n);
    if (client->getSentBytes() >= response.size())
        _removeClient(client_fd);
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
            for (size_t k = 0; k < loc.methods.size(); ++k)
                os << loc.methods[k] << " ";
            os << "\n";
            os << "      root:       " << loc.root << "\n";
            os << "      index:      " << loc.index << "\n";
            os << "      autoindex:  " << (loc.autoindex ? "on" : "off")
               << "\n";
            os << "      upload:     "
               << (loc.upload_path.empty() ? "-" : loc.upload_path) << "\n";
            os << "      redirect:   "
               << (loc.redirect.empty() ? "-" : loc.redirect) << "\n";
        }
    }
    return os;
}
