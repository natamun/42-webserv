/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 19:30:00 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/09 20:00:45 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <sys/types.h>

#include <string>

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Location.hpp"

struct CgiProcess
{
    pid_t pid;
    int pipe_in;
    int pipe_out;
};

CgiProcess spawnCgi(std::string const& uri,
                    HttpRequest const& request,
                    Location const& loc);

#endif
