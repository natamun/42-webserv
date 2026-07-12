/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/12 17:50:48 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/12 17:56:19 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <string>
#include <vector>

#include "HttpRequest.hpp"
#include "Location.hpp"
#include "Server.hpp"

Server const& findServer(std::vector<Server> const& servers,
                         std::string const& host,
                         int client_fd);
Location const* findLocation(std::string const& path, Server const& server);
bool isCgi(HttpRequest const& request, Location const& loc);

#endif
