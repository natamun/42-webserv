/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/04 18:33:44 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/09 02:09:50 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Handlers.hpp"

void handleGet(std::string const& path,
               Server const& server,
               Location const& loc,
               HttpResponse& res)
{
    (void)path;
    (void)server;
    (void)loc;
    res.setStatus(200);
    res.setHeader("Content-Type", "text/html");
    res.setBody("<h1>Hello from webserv!</h1>");
}
