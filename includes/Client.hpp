/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/04 00:02:05 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/04 00:18:08 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <ostream>
#include <string>

class Client
{
public:
    Client();
    Client(int fd, std::string const& ip, int port);
    ~Client();

    int getFd() const;
    std::string const& getIp() const;
    int getPort() const;
    std::string const& getRequest() const;
    std::string const& getResponse() const;

    void appendRequest(char const* buf, size_t n);
    void setResponse(std::string const& response);

private:
    Client(Client const& other);
    Client& operator=(Client const& other);

    int _fd;
    std::string _request;
    std::string _response;
    std::string _ip;
    int _port;
};

std::ostream& operator<<(std::ostream& os, Client const& client);

#endif
