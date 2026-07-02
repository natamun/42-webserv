/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 01:19:56 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/02 20:07:40 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <map>

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
}

void WebServer::run()
{
    _initPollFds();

    while (true)
    {
        int ready = poll(&_pollfds[0], _pollfds.size(), -1);
        if (ready == -1)
            throw WebServerException("poll() failed");

        for (size_t i = 0; i < _pollfds.size(); ++i)
        {
            if (!(_pollfds[i].revents & POLLIN))
                continue;

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

void WebServer::_closeSockets()
{
    for (size_t i = 0; i < _sockets.size(); ++i)
        close(_sockets[i]);
    _sockets.clear();
}

void WebServer::_initPollFds()
{
    for (size_t i = 0; i < _sockets.size(); ++i)
    {
        struct pollfd pfd;
        pfd.fd = _sockets[i];
        pfd.events = POLLIN;
        pfd.revents = 0;
        _pollfds.push_back(pfd);
    }
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
    int client_fd = accept(listening_fd, NULL, NULL);
    if (client_fd == -1)
        throw WebServerException("accept() failed");

    if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1)
    {
        close(client_fd);
        throw WebServerException("fcntl() failed on client fd");
    }

    struct pollfd pfd;
    pfd.fd = client_fd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    _pollfds.push_back(pfd);
}

void WebServer::_readClient(int client_fd)
{
    char buf[4096];

    ssize_t n = recv(client_fd, buf, sizeof(buf), 0);

    if (n <= 0)
    {
        close(client_fd);
        _requests.erase(client_fd);
        for (size_t i = 0; i < _pollfds.size(); ++i)
        {
            if (_pollfds[i].fd == client_fd)
            {
                _pollfds.erase(_pollfds.begin() + i);
                break;
            }
        }
        return;
    }
    if (n > 0)
    {
        _requests[client_fd].append(buf, n);
        if (_requests[client_fd].find("\r\n\r\n") != std::string::npos)
        {
            // We will call HTTP parser but right not we just print the content
            std::cout << "[fd=" << client_fd << "] request complete:\n";
            std::cout << _requests[client_fd] << std::endl;
        }
    }
}
