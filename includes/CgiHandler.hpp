/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 19:30:00 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/10 03:50:00 by nmunari          ###   ########.fr       */
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
    size_t body_sent;
    CgiProcess() : pid(-1), pipe_in(-1), pipe_out(-1), body_sent(0) {}
};

CgiProcess spawnCgi(HttpRequest const& request, Location const& loc);

void buildCgiResponse(std::string const& output, HttpResponse& response);

#endif
