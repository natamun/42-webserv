/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeleteHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/04 18:33:42 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/11 21:56:14 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/stat.h>

#include <cstddef>
#include <cstdio>

#include "Handlers.hpp"

void handleDelete(std::string const& path,
                  Location const& loc,
                  HttpResponse& res)
{
    bool deleteAllowed = false;
    for (size_t i = 0; i < loc.methods.size(); ++i)
    {
        if (loc.methods[i] == "DELETE")
            deleteAllowed = true;
    }
    if (!deleteAllowed)
    {
        res.setStatus(405);
        return;
    }

    if (path.find("..") != std::string::npos)
    {
        res.setStatus(403);
        return;
    }

    std::string relative = path.substr(loc.path.size());
    if (relative.empty() || relative[0] != '/')
        relative = "/" + relative;

    std::string filePath = loc.root + relative;

    struct stat st;
    if (stat(filePath.c_str(), &st) != 0)
    {
        res.setStatus(404);
        return;
    }
    if (S_ISDIR(st.st_mode))
    {
        res.setStatus(403);
        return;
    }

    if (std::remove(filePath.c_str()) != 0)
    {
        res.setStatus(500);
        return;
    }
    res.setStatus(204);
}
