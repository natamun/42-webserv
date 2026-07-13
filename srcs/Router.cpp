/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/12 17:55:09 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/12 17:56:15 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Router.hpp"

#include <netinet/in.h>
#include <sys/socket.h>

#include <cstddef>

Server const& findServer(std::vector<Server> const& servers,
                         std::string const& host,
                         int client_fd)
{
    struct sockaddr_in local_addr;
    socklen_t len = sizeof(local_addr);
    int real_port = -1;

    if (getsockname(client_fd, (struct sockaddr*)&local_addr, &len) == 0)
        real_port = ntohs(local_addr.sin_port);

    std::string hostname = host.substr(0, host.find(":"));
    Server const* fallback = &servers[0];

    for (size_t i = 0; i < servers.size(); ++i)
    {
        if (servers[i].port != real_port)
            continue;
        if (servers[i].server_name == hostname)
            return servers[i];
        fallback = &servers[i];
    }
    return *fallback;
}

Location const* findLocation(std::string const& path, Server const& server)
{
    Location const* best = NULL;
    for (size_t i = 0; i < server.locations.size(); ++i)
    {
        if (path.find(server.locations[i].path) == 0)
        {
            if (!best || server.locations[i].path.size() > best->path.size())
                best = &server.locations[i];
        }
    }
    return best;
}

bool isCgi(HttpRequest const& request, Location const& loc)
{
    if (loc.cgi.empty())
        return false;
    std::string ext = request.getExtension();
    if (ext.empty())
        return false;
    return loc.cgi.count(ext) > 0;
}
