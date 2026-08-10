/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handlers.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/04 18:33:25 by nmunari           #+#    #+#             */
/*   Updated: 2026/08/10 12:48:45 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HANDLERS_HPP
#define HANDLERS_HPP

#include <string>

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Location.hpp"
#include "Server.hpp"

void handleGet(std::string const& path, Location const& loc, HttpResponse& res);

void handlePost(std::string const& path,
                std::string const& body,
                std::string const& contentType,
                Location const& loc,
                HttpResponse& res);

void handleDelete(std::string const& path,
                  Location const& loc,
                  HttpResponse& res);

#endif
