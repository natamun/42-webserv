/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/04 15:39:29 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/07 16:33:16 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"

#include <fstream>
#include <map>
#include <sstream>
#include <string>

HttpResponse::HttpResponse() : _status(200), _version("HTTP/1.1")
{
    _headers["Connection"] = "close";
}

HttpResponse::HttpResponse(HttpResponse const& other)
    : _status(other._status),
      _version(other._version),
      _headers(other._headers),
      _body(other._body)
{
}

HttpResponse::~HttpResponse() {}

HttpResponse& HttpResponse::operator=(HttpResponse const& other)
{
    if (this != &other)
    {
        _status = other._status;
        _version = other._version;
        _headers = other._headers;
        _body = other._body;
    }
    return *this;
}

void HttpResponse::setVersion(std::string const& version)
{
    _version = version;
}

void HttpResponse::setStatus(int code)
{
    _status = code;
}

void HttpResponse::setHeader(std::string const& key, std::string const& value)
{
    _headers[key] = value;
}

void HttpResponse::setBody(std::string const& body)
{
    _body = body;
    std::ostringstream oss;
    oss << body.size();
    _headers["Content-Length"] = oss.str();
}

int HttpResponse::getStatus() const
{
    return _status;
}

void HttpResponse::buildDefaultErrorBody()
{
    std::ostringstream html;
    html << "<html><body><h1>" << _status << " " << _statusMessage()
         << "</h1></body></html>";
    setBody(html.str());
    setHeader("Content-Type", "text/html");
}

std::string HttpResponse::build() const
{
    std::ostringstream oss;
    oss << _version << " " << _status << " " << _statusMessage() << "\r\n";

    if (_headers.find("Content-Length") == _headers.end())
        oss << "Content-Length: 0\r\n";

    std::map<std::string, std::string>::const_iterator it;
    for (it = _headers.begin(); it != _headers.end(); ++it)
        oss << it->first << ": " << it->second << "\r\n";

    oss << "\r\n" << _body;
    return oss.str();
}

std::string HttpResponse::_statusMessage() const
{
    static std::map<int, std::string> statusCodes;
    if (statusCodes.empty())
    {
        statusCodes[200] = "OK";
        statusCodes[201] = "Created";
        statusCodes[204] = "No Content";
        statusCodes[301] = "Moved Permanently";
        statusCodes[302] = "Found";
        statusCodes[400] = "Bad Request";
        statusCodes[403] = "Forbidden";
        statusCodes[404] = "Not Found";
        statusCodes[405] = "Method Not Allowed";
        statusCodes[413] = "Request Entity Too Large";
        statusCodes[500] = "Internal Server Error";
        statusCodes[501] = "Not Implemented";
        statusCodes[503] = "Service Unavailable";
    }

    std::map<int, std::string>::const_iterator it = statusCodes.find(_status);
    if (it != statusCodes.end())
        return it->second;

    return "Unknown";
}

void HttpResponse::setErrorStatus(int code,
                                  std::map<int, std::string> const& errorPages)
{
    _status = code;
    std::map<int, std::string>::const_iterator it = errorPages.find(code);
    if (it != errorPages.end())
    {
        std::ifstream file(it->second.c_str(), std::ios::in | std::ios::binary);
        if (file.is_open())
        {
            std::ostringstream ss;
            ss << file.rdbuf();
            setBody(ss.str());
            setHeader("Content-Type", "text/html");
            return;
        }
    }
    buildDefaultErrorBody();
}
