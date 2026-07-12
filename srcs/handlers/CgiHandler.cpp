/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 01:02:49 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/12 16:37:59 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

static void closePipes(int pipe_in[2], int pipe_out[2])
{
    close(pipe_in[0]);
    close(pipe_in[1]);
    close(pipe_out[0]);
    close(pipe_out[1]);
}

static void runChild(int pipe_in[2],
                     int pipe_out[2],
                     HttpRequest const& request,
                     Location const& loc)
{
    if (dup2(pipe_in[0], STDIN_FILENO) == -1 ||
        dup2(pipe_out[1], STDOUT_FILENO) == -1)
    {
        std::cerr << "[cgi] dup2() failed: " << strerror(errno) << std::endl;
        closePipes(pipe_in, pipe_out);
        exit(1);
    }
    closePipes(pipe_in, pipe_out);
    if (chdir(loc.root.c_str()) == -1)
    {
        std::cerr << "[cgi] chdir() failed: " << strerror(errno) << std::endl;
        exit(1);
    }

    std::string uri_path = request.getPath();
    std::string ext = request.getExtension();
    std::map<std::string, std::string>::const_iterator it = loc.cgi.find(ext);
    std::string interpreter = (it != loc.cgi.end()) ? it->second : "";
    std::string query_string = request.getQueryString();
    std::string script_name = uri_path.substr(loc.path.size());
    if (!script_name.empty() && script_name[0] == '/')
        script_name.erase(0, 1);

    std::string host = request.getHeader("host");
    size_t colon = host.find(':');
    std::string server_name = host.substr(0, colon);
    std::string server_port =
        (colon != std::string::npos) ? host.substr(colon + 1) : "80";

    std::ostringstream oss;
    oss << request.getBody().size();

    std::string method = "REQUEST_METHOD=" + request.getMethod();
    std::string query = "QUERY_STRING=" + query_string;
    std::string script = "SCRIPT_FILENAME=" + script_name;
    std::string script_nm = "SCRIPT_NAME=" + uri_path;
    std::string cont_length = "CONTENT_LENGTH=" + oss.str();
    std::string cont_type = "CONTENT_TYPE=" + request.getHeader("content-type");
    std::string protocol = "SERVER_PROTOCOL=" + request.getVersion();
    std::string srv_name = "SERVER_NAME=" + server_name;
    std::string srv_port = "SERVER_PORT=" + server_port;
    std::string gateway = "GATEWAY_INTERFACE=CGI/1.1";
    std::string redirect = "REDIRECT_STATUS=200";

    char* env[] = {const_cast<char*>(method.c_str()),
                   const_cast<char*>(query.c_str()),
                   const_cast<char*>(script.c_str()),
                   const_cast<char*>(script_nm.c_str()),
                   const_cast<char*>(cont_length.c_str()),
                   const_cast<char*>(cont_type.c_str()),
                   const_cast<char*>(protocol.c_str()),
                   const_cast<char*>(srv_name.c_str()),
                   const_cast<char*>(srv_port.c_str()),
                   const_cast<char*>(gateway.c_str()),
                   const_cast<char*>(redirect.c_str()),
                   NULL};
    char* argv[] = {const_cast<char*>(interpreter.c_str()),
                    const_cast<char*>(script_name.c_str()),
                    NULL};

    execve(interpreter.c_str(), argv, env);
    std::cerr << "[cgi] execve() failed: " << strerror(errno) << std::endl;
    exit(1);
}

CgiProcess spawnCgi(HttpRequest const& request, Location const& loc)
{
    int pipe_in[2];
    int pipe_out[2];
    pid_t pid;
    CgiProcess failed;

    if (pipe(pipe_in) == -1)
    {
        std::cerr << "[cgi] pipe() for pipe_in failed: " << strerror(errno)
                  << std::endl;
        return failed;
    }
    if (pipe(pipe_out) == -1)
    {
        std::cerr << "[cgi] pipe() for pipe_out failed: " << strerror(errno)
                  << std::endl;
        close(pipe_in[0]);
        close(pipe_in[1]);
        return failed;
    }
    pid = fork();
    if (pid == -1)
    {
        std::cerr << "[cgi] fork() failed: " << strerror(errno) << std::endl;
        closePipes(pipe_in, pipe_out);
        return failed;
    }
    if (pid == 0)
        runChild(pipe_in, pipe_out, request, loc);

    close(pipe_in[0]);
    close(pipe_out[1]);

    if (fcntl(pipe_in[1], F_SETFL, O_NONBLOCK) == -1 ||
        fcntl(pipe_out[0], F_SETFL, O_NONBLOCK) == -1)
    {
        std::cerr << "[cgi] fcntl() failed: " << strerror(errno) << std::endl;
        close(pipe_in[1]);
        close(pipe_out[0]);
        return failed;
    }

    CgiProcess proc;
    proc.pid = pid;
    proc.pipe_in = pipe_in[1];
    proc.pipe_out = pipe_out[0];
    proc.started = time(NULL);
    return proc;
}

static bool setSeparator(std::string const& output,
                         size_t& sep,
                         std::string& eol)
{
    sep = output.find("\r\n\r\n");
    if (sep != std::string::npos)
    {
        eol = "\r\n";
        return true;
    }
    sep = output.find("\n\n");
    if (sep != std::string::npos)
    {
        eol = "\n";
        return true;
    }
    return false;
}

void buildCgiResponse(std::string const& output, HttpResponse& response)
{
    size_t sep;
    std::string eol;

    if (!setSeparator(output, sep, eol))
    {
        response.setStatus(502);
        return;
    }

    std::string headers_raw = output.substr(0, sep);
    std::string body = output.substr(sep + eol.size() * 2);

    response.setStatus(200);

    size_t pos = 0;
    while (pos < headers_raw.size())
    {
        size_t end = headers_raw.find(eol, pos);
        if (end == std::string::npos)
            end = headers_raw.size();

        std::string line = headers_raw.substr(pos, end - pos);
        size_t colon = line.find(": ");
        if (colon != std::string::npos)
        {
            std::string key = line.substr(0, colon);
            std::string val = line.substr(colon + 2);
            if (key == "Status")
            {
                std::istringstream iss(val);
                int code;
                iss >> code;
                response.setStatus(code);
            }
            else
                response.setHeader(key, val);
        }
        pos = end + eol.size();
    }

    response.setBody(body);
}
