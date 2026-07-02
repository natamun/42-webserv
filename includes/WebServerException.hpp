/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServerException.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 03:42:50 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/02 16:35:37 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVEREXCEPTION_HPP
#define WEBSERVEREXCEPTION_HPP

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

private:
    static std::string _build(std::string const& func,
                              std::string const& host,
                              int port)
    {
        std::ostringstream oss;
        oss << "Error: " << func << "() failed for " << host << ":" << port;
        return oss.str();
    }
};

#endif
