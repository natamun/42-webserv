/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CookieManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/12 17:27:36 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/16 18:54:39 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COOKIEMANAGER_HPP
#define COOKIEMANAGER_HPP

#include <string>

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class CookieManager
{
public:
    CookieManager();
    ~CookieManager();

    // actions
    std::string ensureClientId(HttpRequest const& request,
                               HttpResponse& response);

private:
    CookieManager(CookieManager const& other);
    CookieManager& operator=(CookieManager const& other);

    std::string _extractCookie(std::string const& cookieHeader,
                               std::string const& name) const;

    size_t _nextClientId;
};

#endif
