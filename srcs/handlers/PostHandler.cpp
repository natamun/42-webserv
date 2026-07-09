/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/04 18:33:46 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/09 02:09:49 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Handlers.hpp"

void handlePost(std::string const& path,
                std::string const& body,
                Server const& server,
                Location const& loc,
                HttpResponse& res)
{
    (void)path;
    (void)body;
    (void)server;
    (void)loc;
    res.setStatus(501);
}
