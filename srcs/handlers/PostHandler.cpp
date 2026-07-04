/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/04 18:33:46 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/04 19:31:47 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Handlers.hpp"

void handlePost(std::string const& path,
                std::string const& body,
                HttpResponse& res)
{
    (void)path;
    (void)body;
    res.setStatus(501);
}
