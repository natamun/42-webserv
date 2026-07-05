/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/04 18:33:46 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/04 23:06:36 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Handlers.hpp"

void handlePost(std::string const& path,
                std::string const& body,
                Server const& server,
                HttpResponse& res)
{
    (void)path;
    (void)body;
    (void)server;
    res.setStatus(501);
}
