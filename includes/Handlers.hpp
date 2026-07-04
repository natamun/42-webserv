/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handlers.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/04 18:33:25 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/04 19:31:53 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HANDLERS_HPP
#define HANDLERS_HPP

#include <string>

#include "HttpResponse.hpp"

void handleGet(std::string const& path, HttpResponse& res);
void handlePost(std::string const& path,
                std::string const& body,
                HttpResponse& res);
void handleDelete(std::string const& path, HttpResponse& res);

#endif
