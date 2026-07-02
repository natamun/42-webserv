/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 01:19:59 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/02 19:08:38 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <poll.h>

#include <ostream>
#include <vector>

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

    void _closeSockets();
    void _initPollFds();
    bool _isListeningSocket(int listening_fd) const;
    void _acceptClient(int listening_fd);
    void _readClient(int client_fd);

    std::vector<Server> _servers;
    std::vector<int> _sockets;
    std::vector<struct pollfd> _pollfds;
};

std::ostream& operator<<(std::ostream& os, WebServer const& ws);

#endif