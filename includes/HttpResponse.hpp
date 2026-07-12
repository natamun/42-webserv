/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/04 15:39:25 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/12 17:41:59 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <map>
#include <string>

class HttpResponse
{
public:
    HttpResponse();
    HttpResponse(HttpResponse const& other);
    ~HttpResponse();
    HttpResponse& operator=(HttpResponse const& other);

    void setStatus(int code);
    void setVersion(std::string const& version);
    void setHeader(std::string const& key, std::string const& value);
    void setBody(std::string const& body);
    void buildDefaultErrorBody();

    std::string build() const;
    int getStatus() const;

private:
    std::string _statusMessage() const;

    int _status;
    std::string _version;
    std::map<std::string, std::string> _headers;
    std::string _body;
};

#endif
