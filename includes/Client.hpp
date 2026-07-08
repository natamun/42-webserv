/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/04 00:02:05 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/07 16:16:44 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <ostream>
#include <string>

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

    void appendRequest(char const* buf, size_t n);
    void setResponse(std::string const& response);
    void addSentBytes(size_t n);

private:
    Client(Client const& other);
    Client& operator=(Client const& other);

    int _fd;
    HttpRequest _request;
    std::string _response;
    std::string _ip;
    int _port;
    size_t _sent_bytes;
};

std::ostream& operator<<(std::ostream& os, Client const& client);

#endif
