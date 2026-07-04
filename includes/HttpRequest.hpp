/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dufama <dufama@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/04 16:26:47 by dufama            #+#    #+#             */
/*   Updated: 2026/07/04 16:26:47 by dufama           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <cstdlib>
#include <map>
#include <string>

class HttpRequest
{
public:
    enum State
    {
        PARSING_REQUEST_LINE,
        PARSING_HEADERS,
        PARSING_BODY,
        COMPLETE,
        ERROR
    };

    HttpRequest();
    ~HttpRequest();

    void appendData(char const* buf, size_t len);
    bool isComplete() const;
    bool hasError() const;

    std::string const& getMehtod() const;
    std::string const& getUri() const;
    std::string const& getVersion() const;
    std::map<std::string, std::string> const& getHeaders() const;
    std::string const& getBody() const;

private:
    State _state;
    std::string _buffer;
    std::string _method;
    std::string _uri;
    std::string _version;
    std::map<std::string, std::string> _headers;
    std::string _body;
    size_t _contentLength;

    void _tryParseRequestLine();
    void _tryParseHeaders();
    void _tryParseBody();
    void _fail();
};

#endif