/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/04 18:33:46 by nmunari           #+#    #+#             */
/*   Updated: 2026/08/10 12:48:42 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

#include "Handlers.hpp"

static std::string extractBoundary(std::string const& contentType)
{
    size_t pos = contentType.find("boundary=");
    if (pos == std::string::npos)
        return "";
    std::string boundary = contentType.substr(pos + 9);
    size_t end = boundary.find(';');
    if (end != std::string::npos)
        boundary = boundary.substr(0, end);
    return boundary;
}

static std::string extractFilename(std::string const& disposition)
{
    size_t pos = disposition.find("filename=\"");
    if (pos == std::string::npos)
        return "";
    pos += 10;
    size_t end = disposition.find('"', pos);
    if (end == std::string::npos)
        return "";
    return disposition.substr(pos, end - pos);
}

static bool parseMultipart(std::string const& body,
                           std::string const& boundary,
                           std::string& filename,
                           std::string& fileContent)
{
    std::string delimiter = "--" + boundary;

    size_t partStart = body.find(delimiter + "\r\n");
    if (partStart == std::string::npos)
        return false;
    partStart += delimiter.size() + 2;

    size_t headerEnd = body.find("\r\n\r\n", partStart);
    if (headerEnd == std::string::npos)
        return false;

    std::string headers = body.substr(partStart, headerEnd - partStart);

    size_t cdPos = headers.find("Content-Disposition:");
    if (cdPos != std::string::npos)
    {
        size_t lineEnd = headers.find("\r\n", cdPos);
        std::string cd = headers.substr(cdPos, lineEnd - cdPos);
        filename = extractFilename(cd);
    }

    size_t contentStart = headerEnd + 4;
    size_t contentEnd = body.find("\r\n" + delimiter, contentStart);
    if (contentEnd == std::string::npos)
        return false;

    fileContent = body.substr(contentStart, contentEnd - contentStart);
    return true;
}

void handlePost(std::string const& path,
                std::string const& body,
                std::string const& contentType,
                Location const& loc,
                HttpResponse& res)
{
    if (loc.upload_path.empty())
    {
        res.setStatus(500);
        return;
    }

    std::string filename = path.substr(loc.path.size());
    std::string fileContent = body;

    if (filename.find("..") != std::string::npos)
    {
        res.setStatus(403);
        return;
    }

    if (contentType.find("multipart/form-data") != std::string::npos)
    {
        std::string boundary = extractBoundary(contentType);
        std::string multipartFilename;
        if (boundary.empty() ||
            !parseMultipart(body, boundary, multipartFilename, fileContent))
        {
            res.setStatus(400);
            return;
        }
        if (filename.empty() || filename == "/")
            filename = "/" + multipartFilename;
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

    outFile.write(fileContent.c_str(), fileContent.size());
    outFile.close();

    res.setStatus(201);
    res.setHeader("Content-Type", "text/html");
    res.setBody("<h1> 201 Created</h1><p>File uploaded: " + filename + "</p>");
}
