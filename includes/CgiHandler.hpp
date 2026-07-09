/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 19:30:00 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/09 21:47:57 by nmunari          ###   ########.fr       */
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
    CgiProcess() : pid(-1), pipe_in(-1), pipe_out(-1) {}
};

CgiProcess spawnCgi(HttpRequest const& request, Location const& loc);

void buildCgiResponse(std::string const& output, HttpResponse& response);

#endif
