/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/04 18:33:46 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/11 18:23:02 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

#include "Handlers.hpp"

void handlePost(std::string const& path,
                std::string const& body,
                Location const& loc,
                HttpResponse& res)
{
    bool postAllowed = false;
    for (size_t i = 0; i < loc.methods.size(); ++i)
    {
        if (loc.methods[i] == "POST")
            postAllowed = true;
    }
    if (!postAllowed)
    {
        res.setStatus(405);
        return;
    }
    if (loc.upload_path.empty())
    {
        res.setStatus(500);
        return;
    }

    std::string filename = path.substr(loc.path.size());
    if (filename.find("..") != std::string::npos)
    {
        res.setStatus(403);
        return;
    }
    if (filename.empty() || filename == "/")
    {
        std::ostringstream oss;
        oss << time(NULL);
        filename = "/upload_" + oss.str();
    }

    std::string filePath = loc.upload_path + filename;

    std::ofstream outFile(filePath.c_str(),
                          std::ios::out | std::ios::binary | std::ios::trunc);
    if (!outFile.is_open())
    {
        res.setStatus(500);
        return;
    }

    outFile << body;
    outFile.close();

    res.setStatus(201);
    res.setHeader("Content-Type", "text/html");
    res.setBody("<h1> 201 Created</h1><p>File uploaded: " + filename + "</p>");
}
