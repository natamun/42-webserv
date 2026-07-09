/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 01:02:49 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/09 01:33:06 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Handlers.hpp"

void handleCgi(std::string const& uri,
               HttpRequest const& request,
               Location const& loc,
               HttpResponse& response)
{
    (void)uri;
    (void)request;
    (void)loc;
    response.setStatus(501);
}
