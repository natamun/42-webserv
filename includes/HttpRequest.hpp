/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/04 16:26:47 by dufama            #+#    #+#             */
/*   Updated: 2026/07/10 02:23:06 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <cstdlib>
#include <map>
#include <ostream>
#include <string>

class HttpRequest
{
public:
    enum State
    {
        PARSING_REQUEST_LINE,
        PARSING_HEADERS,
        PARSING_BODY,
        PARSING_CHUNKED_BODY,
        COMPLETE,
        ERROR
    };

    HttpRequest();
    ~HttpRequest();

    void appendData(char const* buf, size_t len);
    bool isComplete() const;
    bool hasError() const;

    std::string const& getMethod() const;
    std::string const& getUri() const;
    std::string const& getVersion() const;
    std::string const& getBody() const;
    std::map<std::string, std::string> const& getHeaders() const;
    std::string const& getHeader(std::string const& key) const;
    std::string const& getError() const;

private:
    State _state;
    std::string _buffer;
    std::string _method;
    std::string _uri;
    std::string _version;
    std::map<std::string, std::string> _headers;
    std::string _body;
    std::string _error;
    size_t _contentLength;

    void _tryParseRequestLine();
    void _tryParseHeaders();
    void _tryParseBody();
    void _tryParseChunkedBody();
    void _fail(std::string const& reason);
};

std::ostream& operator<<(std::ostream& os, HttpRequest const& req);

#endif