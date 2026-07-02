/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 01:19:56 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/02 04:21:59 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"

#include <arpa/inet.h>
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

        _sockets.push_back(fd);
    }
}

WebServer::~WebServer()
{
    _closeSockets();
}

void WebServer::run() {}

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
