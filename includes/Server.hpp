/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 00:35:44 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/02 00:36:43 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <string>
#include <vector>

#include "Location.hpp"

class Server
{
public:
    std::string host;
    int port;
    int client_max_body_size;
    std::map<int, std::string> error_pages;
    std::vector<Location> locations;
};

#endif