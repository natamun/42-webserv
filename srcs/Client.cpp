/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/04 00:00:00 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/12 17:40:51 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client() : _fd(-1), _port(0), _sent_bytes(0), _cgi(), _has_cgi(false) {}

Client::Client(int fd, std::string const& ip, int port)
    : _fd(fd), _ip(ip), _port(port), _sent_bytes(0), _cgi(), _has_cgi(false)
{
}

Client::~Client() {}

void Client::appendRequest(char const* buf, size_t n)
{
    _request.appendData(buf, n);
}

void Client::checkBodyLimit(size_t maxBodySize)
{
    _request.checkBodyLimit(maxBodySize);
}

void Client::setResponse(std::string const& response)
{
    _response = response;
    _sent_bytes = 0;
}

void Client::addSentBytes(size_t n)
{
    _sent_bytes += n;
}

void Client::setCgi(CgiProcess const& cgi)
{
    _cgi = cgi;
    _has_cgi = true;
}

void Client::addCgiBodySent(size_t n)
{
    _cgi.body_sent += n;
}

void Client::appendCgiOutput(char const* buf, size_t n)
{
    _cgi_output.append(buf, n);
}

void Client::clearCgi()
{
    _cgi = CgiProcess();
    _cgi_output.clear();
    _has_cgi = false;
}

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

HttpRequest const& Client::getRequest() const
{
    return _request;
}

std::string const& Client::getResponse() const
{
    return _response;
}

size_t Client::getSentBytes() const
{
    return _sent_bytes;
}

CgiProcess const& Client::getCgi() const
{
    return _cgi;
}

std::string const& Client::getCgiOutput() const
{
    return _cgi_output;
}

bool Client::hasCgi() const
{
    return _has_cgi;
}

std::ostream& operator<<(std::ostream& os, Client const& client)
{
    os << "[client fd=" << client.getFd() << " ip=" << client.getIp() << ":"
       << client.getPort() << "]";
    return os;
}
