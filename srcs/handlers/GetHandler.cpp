/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/04 18:33:44 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/04 23:06:38 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Handlers.hpp"

void handleGet(std::string const& path, Server const& server, HttpResponse& res)
{
    // TODO: read file from disk based on path and server.root
    (void)path;
    (void)server;
    res.setStatus(200);
    res.setHeader("Content-Type", "text/html");
    res.setBody("<h1>Hello from webserv!</h1>");
}
