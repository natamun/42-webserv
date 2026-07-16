/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CookieManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/12 17:27:36 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/16 19:13:44 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COOKIEMANAGER_HPP
#define COOKIEMANAGER_HPP

#include <map>
#include <string>

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class CookieManager
{
public:
    CookieManager();
    ~CookieManager();

    std::string ensureClientId(HttpRequest const& request,
                               HttpResponse& response);
    void trackRequest(std::string const& id, HttpResponse& response);

private:
    CookieManager(CookieManager const& other);
    CookieManager& operator=(CookieManager const& other);

    std::string _extractCookie(std::string const& cookieHeader,
                               std::string const& name) const;

    size_t _nextClientId;
    std::map<std::string, int> _requestCounts;
};

#endif
