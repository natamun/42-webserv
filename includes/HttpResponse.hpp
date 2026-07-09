/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/04 15:39:25 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/07 16:33:18 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <map>
#include <string>

// HTTP/1.1 {_status} {_statusMessage()}\r\n
// {_headers[key]}: {_headers[value]}\r\n  (repeated for each header)
// \r\n
// {_body}
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

    int getStatus() const;
    void buildDefaultErrorBody();
    void setErrorStatus(int code, std::map<int, std::string> const& errorPages);

    std::string build() const;

private:
    std::string _statusMessage() const;

    int _status;
    std::string _version;
    std::map<std::string, std::string> _headers;
    std::string _body;
};

#endif
