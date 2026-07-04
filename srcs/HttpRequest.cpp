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
#include <sstream>

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
