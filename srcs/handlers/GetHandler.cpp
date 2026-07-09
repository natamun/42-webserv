/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/04 18:33:44 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/09 02:09:50 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/stat.h>

#include <cstddef>
#include <fstream>
#include <sstream>

#include "Handlers.hpp"

void handleGet(std::string const& path,
               Server const& server,
               Location const& loc,
               HttpResponse& res)
{
    (void)server;
    bool getAllowed = false;
    for (size_t i = 0; i < loc.methods.size(); ++i)
    {
        if (loc.methods[i] == "GET")
            getAllowed = true;
    }
    if (!getAllowed)
    {
        res.setStatus(405);
        return;
    }

    std::string filePath = loc.root + path.substr(loc.path.size());

    struct stat st;
    if (stat(filePath.c_str(), &st) != 0)
    {
        res.setStatus(404);
        return;
    }

    if (S_ISDIR(st.st_mode))
    {
        if (!filePath.empty() && filePath[filePath.size() - 1] != '/')
            filePath += "/";
        if (!loc.index.empty())
            filePath += loc.index;

        if (stat(filePath.c_str(), &st) != 0 || S_ISDIR(st.st_mode))
        {
            res.setStatus(403);
            return;
        }
    }

    std::ifstream file(filePath.c_str(), std::ios::in | std::ios::binary);
    if (!file.is_open())
    {
        res.setStatus(404);
        return;
    }
    std::ostringstream ss;
    ss << file.rdbuf();

    res.setStatus(200);
    res.setHeader("Content-Type", "coucou");
    res.setBody(ss.str());
}
