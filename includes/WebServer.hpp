/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 01:19:59 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/04 22:53:20 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <poll.h>

#include <map>
#include <ostream>
#include <vector>

#include "Client.hpp"
#include "HttpResponse.hpp"
#include "Server.hpp"

class WebServer
{
public:
    WebServer(std::vector<Server> const& servers);
    ~WebServer();

    void run();

    std::vector<Server> const& getServers() const;
    std::vector<int> const& getSockets() const;

private:
    WebServer(WebServer const& other);
    WebServer& operator=(WebServer const& other);

    // sockets
    void _initPollFds();
    void _closeSockets();

    // poll
    int _findPollFd(int fd) const;
    void _addPollFd(int fd, short events);
    void _setPollEvent(int fd, short events);

    // clients
    void _removeClient(int client_fd);
    void _closeClients();

    // routing
    Server const& _findServer(std::string const& host) const;

    // event loop
    bool _isListeningSocket(int listening_fd) const;
    void _acceptClient(int listening_fd);
    void _readClient(int client_fd);
    void _handleRequest(int client_fd);
    void _handleError(HttpResponse& res,
                      Server const& server,
                      std::string const& client_ip);
    void _sendResponse(int client_fd);

    std::vector<Server> _servers;
    std::vector<int> _sockets;
    std::vector<struct pollfd> _pollfds;
    std::map<int, Client*> _clients;
};

std::ostream& operator<<(std::ostream& os, WebServer const& ws);

#endif