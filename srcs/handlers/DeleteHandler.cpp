/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeleteHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/04 18:33:42 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/04 19:31:52 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Handlers.hpp"

void handleDelete(std::string const& path, HttpResponse& res)
{
    (void)path;
    res.setStatus(501);
}
