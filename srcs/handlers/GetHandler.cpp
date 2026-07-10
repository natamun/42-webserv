/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dufama <dufama@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/10 10:13:21 by dufama            #+#    #+#             */
/*   Updated: 2026/07/10 10:13:21 by dufama           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/stat.h>

#include <cstddef>
#include <fstream>
#include <map>
#include <sstream>
#include <string>

#include "Handlers.hpp"

static std::string getMimeType(std::string const& path)
{
    static std::map<std::string, std::string> mimeTypes;
    if (mimeTypes.empty())
    {
        mimeTypes[".html"] = "text/html";
        mimeTypes[".htm"] = "text/html";
        mimeTypes[".css"] = "text/css";
        mimeTypes[".js"] = "application/javascript";
        mimeTypes[".json"] = "application/json";
        mimeTypes[".png"] = "image/png";
        mimeTypes[".jpg"] = "image/jpeg";
        mimeTypes[".jpeg"] = "image/jpeg";
        mimeTypes[".gif"] = "image/gif";
        mimeTypes[".txt"] = "text/plain";
        mimeTypes[".pdf"] = "application/pdf";
    }
    size_t dot = path.find_last_of('.');
    if (dot == std::string::npos)
        return "application/octect-stream";

    std::string ext = path.substr(dot);
    std::map<std::string, std::string>::const_iterator it = mimeTypes.find(ext);
    if (it == mimeTypes.end())
        return "application/octct-stream";

    return it->second;
}

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

    std::string filePath = loc.root + path;

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
    res.setHeader("Content-Type", getMimeType(filePath));
    res.setBody(ss.str());
}
