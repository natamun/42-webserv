/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dufama <dufama@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/04 16:57:02 by dufama            #+#    #+#             */
/*   Updated: 2026/07/04 16:57:02 by dufama           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

#include <cstddef>
#include <map>
#include <sstream>
#include <string>

HttpRequest::HttpRequest() : _state(PARSING_REQUEST_LINE), _contentLength(0) {}

HttpRequest::~HttpRequest() {}

void HttpRequest::appendData(char const* buf, size_t len)
{
    if (_state == ERROR || _state == COMPLETE)
        return;

    _buffer.append(buf, len);
    if (_state == PARSING_REQUEST_LINE)
        _tryParseRequestLine();
    if (_state == PARSING_HEADERS)
        _tryParseHeaders();
    if (_state == PARSING_BODY)
        _tryParseBody();
}

void HttpRequest::_tryParseRequestLine()
{
    size_t pos = _buffer.find("\r\n");
    if (pos == std::string::npos)
        return;

    std::string line = _buffer.substr(0, pos);
    _buffer.erase(0, pos + 2);

    std::istringstream iss(line);
    if (!(iss >> _method >> _uri >> _version))
    {
        _fail();
        return;
    }

    std::string extra;
    if (iss >> extra)
    {
        _fail();
        return;
    }

    _state = PARSING_HEADERS;
}

void HttpRequest::_tryParseHeaders()
{
    size_t pos;
    while ((pos = _buffer.find("\r\n")) != std::string::npos)
    {
        std::string line = _buffer.substr(0, pos);

        if (line.empty())
        {
            _buffer.erase(0, pos + 2);

            std::map<std::string, std::string>::const_iterator it =
                _headers.find("Content-Length");
            if (it != _headers.end())
            {
                _contentLength = std::atoi(it->second.c_str());
                _state = PARSING_BODY;
            }
            else
            {
                _state = COMPLETE;
            }
            return;
        }

        size_t colon = line.find(":");
        if (colon == std::string::npos)
        {
            _fail();
            return;
        }

        std::string key = line.substr(0, colon);
        std::string value = line.substr(colon + 1);

        size_t i = value.find_first_not_of("\t");
        if (i != std::string::npos)
            value = value.substr(i);
        else
            value.clear();

        _headers[key] = value;
        _buffer.erase(0, pos + 2);
    }
}

void HttpRequest::_tryParseBody()
{
    if (_buffer.size() < _contentLength)
        return;

    _body = _buffer.substr(0, _contentLength);
    _buffer.erase(0, _contentLength);
    _state = COMPLETE;
}

void HttpRequest::_fail()
{
    _state = ERROR;
}

bool HttpRequest::isComplete() const
{
    return _state == COMPLETE;
}
bool HttpRequest::hasError() const
{
    return _state == ERROR;
}

std::string const& HttpRequest::getMehtod() const
{
    return _method;
}
std::string const& HttpRequest::getUri() const
{
    return _uri;
}
std::string const& HttpRequest::getBody() const
{
    return _body;
}
std::string const& HttpRequest::getVersion() const
{
    return _version;
}

std::string const& HttpRequest::getHeader(std::string const& key) const
{
    static std::string empty;
    std::map<std::string, std::string>::const_iterator it = _headers.find(key);
    if (it == _headers.end())
        return empty;
    return it->second;
}