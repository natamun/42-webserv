/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 01:19:59 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/12 17:56:18 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <poll.h>

#include <map>
#include <ostream>
#include <vector>

#include "Client.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Location.hpp"
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

    // event loop
    bool _isListeningSocket(int listening_fd) const;
    void _acceptClient(int listening_fd);
    void _readClient(int client_fd);
    void _handleRequest(int client_fd);
    bool _isValidRequest(HttpRequest const& request,
                         Server const& server,
                         HttpResponse& response);
    void _handleMethod(int client_fd,
                       HttpRequest const& request,
                       Server const& server,
                       HttpResponse& response);
    void _handleError(HttpResponse& res,
                      Server const& server,
                      std::string const& client_ip);
    void _finalizeResponse(int client_fd,
                           HttpResponse& response,
                           Server const& server);
    void _sendResponse(int client_fd);

    // cgi
    void _handleCgi(int client_fd,
                    HttpRequest const& request,
                    Location const& loc,
                    HttpResponse& response);
    void _removeCgiPipe(int cgi_fd);
    void _readCgiOutput(int cgi_fd);
    void _writeCgiBody(int cgi_fd);
    void _checkCgiTimeouts();
    void _killCgi(int client_fd);

    static const int CGI_TIMEOUT_SECONDS = 5;
    static const int POLL_TIMEOUT_MS = 1000;

    std::vector<Server> _servers;
    std::vector<int> _sockets;
    std::vector<struct pollfd> _pollfds;
    std::map<int, Client*> _clients;
    std::map<int, int> _pipe_to_client;
};

std::ostream& operator<<(std::ostream& os, WebServer const& ws);

#endif