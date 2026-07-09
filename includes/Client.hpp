/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/04 00:02:05 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/09 21:13:33 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <ostream>
#include <string>

#include "CgiHandler.hpp"
#include "HttpRequest.hpp"

class Client
{
public:
    Client();
    Client(int fd, std::string const& ip, int port);
    ~Client();

    int getFd() const;
    std::string const& getIp() const;
    int getPort() const;
    HttpRequest const& getRequest() const;
    std::string const& getResponse() const;
    size_t getSentBytes() const;
    CgiProcess const& getCgi() const;
    std::string const& getCgiOutput() const;
    bool hasCgi() const;

    void appendRequest(char const* buf, size_t n);
    void setResponse(std::string const& response);
    void addSentBytes(size_t n);
    void setCgi(CgiProcess const& cgi);
    void appendCgiOutput(char const* buf, size_t n);
    void clearCgi();

private:
    Client(Client const& other);
    Client& operator=(Client const& other);

    int _fd;
    HttpRequest _request;
    std::string _response;
    std::string _ip;
    int _port;
    size_t _sent_bytes;
    CgiProcess _cgi;
    std::string _cgi_output;
    bool _has_cgi;
};

std::ostream& operator<<(std::ostream& os, Client const& client);

#endif
