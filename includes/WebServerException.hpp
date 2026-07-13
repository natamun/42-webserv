/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServerException.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 03:42:50 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/12 19:02:10 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVEREXCEPTION_HPP
#define WEBSERVEREXCEPTION_HPP

#include <netdb.h>

#include <cerrno>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <string>

class WebServerException : public std::runtime_error
{
public:
    WebServerException(std::string const& msg) : std::runtime_error(msg) {}
    WebServerException(std::string const& func,
                       std::string const& host,
                       int port)
        : std::runtime_error(_build(func, host, port))
    {
    }
    WebServerException(std::string const& func,
                       std::string const& host,
                       int port,
                       int gaiErr)
        : std::runtime_error(_buildGai(func, host, port, gaiErr))
    {
    }

private:
    static std::string _build(std::string const& func,
                              std::string const& host,
                              int port)
    {
        std::ostringstream oss;
        oss << "Error: " << func << "() failed for " << host << ":" << port
            << " : " << strerror(errno);
        return oss.str();
    }

    static std::string _buildGai(std::string const& func,
                                 std::string const& host,
                                 int port,
                                 int gaiErr)
    {
        std::ostringstream oss;
        oss << "Error: " << func << "() failed for " << host << ":" << port
            << " : " << gai_strerror(gaiErr);
        return oss.str();
    }
};

#endif
