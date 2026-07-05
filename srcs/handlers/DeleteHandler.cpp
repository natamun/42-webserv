/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeleteHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/04 18:33:42 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/04 23:06:40 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Handlers.hpp"

void handleDelete(std::string const& path,
                  Server const& server,
                  HttpResponse& res)
{
    (void)path;
    (void)server;
    res.setStatus(501);
}
