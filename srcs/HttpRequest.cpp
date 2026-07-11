/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/04 16:57:02 by dufama            #+#    #+#             */
/*   Updated: 2026/07/11 21:46:06 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

#include <map>
#include <ostream>
#include <sstream>
#include <string>

HttpRequest::HttpRequest()
    : _state(PARSING_REQUEST_LINE), _errorStatus(400), _contentLength(0)
{
}

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
    if (_state == PARSING_CHUNKED_BODY)
        _tryParseChunkedBody();
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
        _fail("Malformed request line: missing method, URI or version");
        return;
    }

    std::string extra;
    if (iss >> extra)
    {
        _fail("Malformed request line: unexpected extra token after version");
        return;
    }

    _state = PARSING_HEADERS;
}

// ===== Static func for TryParseHeaders =====
static std::string toLower(std::string string)
{
    for (size_t i = 0; i < string.size(); ++i)
    {
        string[i] = static_cast<char>(
            std::tolower(static_cast<unsigned char>(string[i])));
    }
    return string;
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
                _headers.find("content-length");
            if (it != _headers.end())
            {
                std::istringstream iss(it->second);
                long len;
                if (!(iss >> len) || len < 0)
                {
                    _fail("Invalid Content-Length value");
                    return;
                }
                _contentLength = static_cast<size_t>(len);
                _state = PARSING_BODY;
            }
            else if (getHeader("transfer-encoding") == "chunked")
            {
                _state = PARSING_CHUNKED_BODY;
            }
            else
            {
                _state = COMPLETE;
            }
            return;
        }

        size_t sep = line.find(":");
        if (sep == std::string::npos)
        {
            _fail("Malformed header line: missing colon");
            return;
        }

        std::string key = toLower(line.substr(0, sep));
        std::string value = line.substr(sep + 1);

        size_t i = value.find_first_not_of(" \t");
        if (i != std::string::npos)
            value = value.substr(i);
        else
            value.clear();

        _headers[key] = value;
        _buffer.erase(0, pos + 2);
    }
}

void HttpRequest::_tryParseChunkedBody()
{
    while (true)
    {
        size_t crlf = _buffer.find("\r\n");
        if (crlf == std::string::npos)
            return;

        size_t chunk_size = strtoul(_buffer.substr(0, crlf).c_str(), NULL, 16);

        if (_buffer.size() < crlf + 2 + chunk_size + 2)
            return;

        _buffer.erase(0, crlf + 2);

        if (chunk_size == 0)
        {
            _state = COMPLETE;
            return;
        }

        _body.append(_buffer, 0, chunk_size);
        _buffer.erase(0, chunk_size + 2);
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

void HttpRequest::_fail(std::string const& reason, int status)
{
    _state = ERROR;
    _error = reason;
    _errorStatus = status;
}

void HttpRequest::checkBodyLimit(size_t maxBodySize)
{
    if (_contentLength > maxBodySize)
        _fail("Body exceeds client_max_body_size", 413);
}

bool HttpRequest::isComplete() const
{
    return _state == COMPLETE;
}
bool HttpRequest::hasError() const
{
    return _state == ERROR;
}

std::string const& HttpRequest::getMethod() const
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

std::map<std::string, std::string> const& HttpRequest::getHeaders() const
{
    return _headers;
}

std::string const& HttpRequest::getHeader(std::string const& key) const
{
    static std::string empty;
    std::map<std::string, std::string>::const_iterator it =
        _headers.find(toLower(key));
    if (it == _headers.end())
        return empty;
    return it->second;
}

std::string const& HttpRequest::getError() const
{
    return _error;
}

int HttpRequest::getErrorStatus() const
{
    return _errorStatus;
}

size_t HttpRequest::getContentLength() const
{
    return _contentLength;
}

std::string HttpRequest::getPath() const
{
    size_t q = _uri.find('?');
    return _uri.substr(0, q);
}

std::string HttpRequest::getQueryString() const
{
    size_t q = _uri.find('?');
    if (q == std::string::npos)
        return "";
    return _uri.substr(q + 1);
}

std::string HttpRequest::getExtension() const
{
    std::string path = getPath();
    size_t slash = path.rfind('/');
    size_t dot = path.rfind('.');
    if (dot == std::string::npos)
        return "";
    if (slash != std::string::npos && dot < slash)
        return "";
    return path.substr(dot);
}

std::ostream& operator<<(std::ostream& os, HttpRequest const& req)
{
    os << req.getMethod() << " " << req.getUri() << " " << req.getVersion()
       << "\n";
    std::map<std::string, std::string> const& headers = req.getHeaders();
    std::map<std::string, std::string>::const_iterator it;
    for (it = headers.begin(); it != headers.end(); ++it)
        os << it->first << ": " << it->second << "\n";
    if (!req.getBody().empty())
        os << "body (" << req.getBody().size() << " bytes):\n"
           << "  " << req.getBody() << "\n";
    if (req.hasError())
        os << "[ERROR] " << req.getError() << "\n";
    return os;
}