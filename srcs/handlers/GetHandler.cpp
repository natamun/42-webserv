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

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <cstddef>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

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
        return "application/octet-stream";

    std::string ext = path.substr(dot);
    std::map<std::string, std::string>::const_iterator it = mimeTypes.find(ext);
    if (it == mimeTypes.end())
        return "application/octet-stream";

    return it->second;
}

static bool isImageExtension(std::string const& name)
{
    size_t dot = name.find_last_of('.');
    if (dot == std::string::npos)
        return false;
    std::string extension = name.substr(dot);
    return extension == ".png" || extension == ".jpg" || extension == ".jpeg" ||
           extension == ".gif";
}

static std::string generateAutoIndex(std::string const& dirPath,
                                     std::string const& uri)
{
    DIR* dir = opendir(dirPath.c_str());
    if (!dir)
        return "";

    std::vector<std::string> entries;
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        std::string name = entry->d_name;
        if (name == ".")
            continue;
        entries.push_back(name);
    }
    closedir(dir);

    std::sort(entries.begin(), entries.end());

    std::ostringstream html;
    html << "<!DOCTYPE html><html><head><meta charset=\"UTF-8\">"
         << "<title>Index of " << uri << "</title></head><body>"
         << "<h1>Index of " << uri << "</h1>";
    for (size_t i = 0; i < entries.size(); ++i)
    {
        std::string href = uri;
        if (!href.empty() && href[href.size() - 1] != '/')
            href += '/';
        href += entries[i];

        if (isImageExtension(entries[i]))
            html << "<img src=\"" << href << "\" width=\"150\">";
        else
            html << "<a href=\"" << href << "\">" << entries[i] << "</a><br>";
    }

    html << "</body></html>";
    return html.str();
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
    std::cerr << "[DEBUG] cwd-relative filePath: " << filePath << std::endl;

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
        bool autoIndex = false;
        if (!loc.index.empty())
        {
            std::string indexPath = filePath + loc.index;
            struct stat indexSt;
            if (stat(indexPath.c_str(), &indexSt) == 0 &&
                !S_ISDIR(indexSt.st_mode))
            {
                filePath = indexPath;
                autoIndex = true;
            }
        }
        if (!autoIndex)
        {
            if (loc.autoindex)
            {
                std::string all = generateAutoIndex(filePath, path);
                res.setStatus(200);
                res.setHeader("Content-Type", "text/html");
                res.setBody(all);
                return;
            }
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
