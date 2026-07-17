/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CookieManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/12 17:31:22 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/16 19:13:36 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CookieManager.hpp"

#include <sstream>

CookieManager::CookieManager() : _nextClientId(0) {}

CookieManager::~CookieManager() {}

std::string CookieManager::ensureClientId(HttpRequest const& request,
                                          HttpResponse& response)
{
    std::string id = _extractCookie(request.getHeader("cookie"), "client_id");

    if (id.empty())
    {
        std::ostringstream oss;
        oss << "client" << ++_nextClientId;
        id = oss.str();
        response.setHeader("Set-Cookie", "client_id=" + id);
    }

    return id;
}

void CookieManager::trackRequest(std::string const& id, HttpResponse& response)
{
    std::ostringstream countStr;
    countStr << ++_requestCounts[id];
    response.setHeader("Request-Count", countStr.str());
}

std::string CookieManager::_extractCookie(std::string const& cookieHeader,
                                          std::string const& name) const
{
    std::string key = name + "=";
    size_t pos = cookieHeader.find(key);
    if (pos == std::string::npos)
        return "";
    pos += key.size();
    size_t end = cookieHeader.find(';', pos);
    return cookieHeader.substr(pos, end - pos);
}
