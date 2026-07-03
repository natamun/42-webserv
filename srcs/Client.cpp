/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/04 00:00:00 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/04 00:18:06 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client() : _fd(-1), _port(0) {}

Client::Client(int fd, std::string const& ip, int port)
    : _fd(fd), _ip(ip), _port(port)
{
}

Client::~Client() {}

int Client::getFd() const
{
    return _fd;
}
std::string const& Client::getIp() const
{
    return _ip;
}
int Client::getPort() const
{
    return _port;
}
std::string const& Client::getRequest() const
{
    return _request;
}
std::string const& Client::getResponse() const
{
    return _response;
}

void Client::appendRequest(char const* buf, size_t n)
{
    _request.append(buf, n);
}

void Client::setResponse(std::string const& response)
{
    _response = response;
}

std::ostream& operator<<(std::ostream& os, Client const& client)
{
    os << "[client fd=" << client.getFd() << " ip=" << client.getIp() << ":"
       << client.getPort() << "]\n";
    os << "  request:\n" << client.getRequest() << "\n";
    os << "  response:\n" << client.getResponse() << "\n";
    return os;
}
