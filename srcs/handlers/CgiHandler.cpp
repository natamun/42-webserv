/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 01:02:49 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/09 20:00:41 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"

#include <unistd.h>

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>

static void closePipes(int pipe_in[2], int pipe_out[2])
{
    close(pipe_in[0]);
    close(pipe_in[1]);
    close(pipe_out[0]);
    close(pipe_out[1]);
}

static void runChild(int pipe_in[2],
                     int pipe_out[2],
                     std::string const& uri,
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
    chdir(loc.root.c_str());

    size_t q = uri.find('?');
    std::string uri_path = uri.substr(0, q);
    std::string query_string =
        (q != std::string::npos) ? uri.substr(q + 1) : "";
    std::string script_name = uri_path.substr(uri_path.rfind('/') + 1);

    std::ostringstream oss;
    oss << request.getBody().size();

    std::string method = "REQUEST_METHOD=" + request.getMethod();
    std::string query = "QUERY_STRING=" + query_string;
    std::string script = "SCRIPT_FILENAME=" + script_name;
    std::string path_info = "PATH_INFO=" + uri;
    std::string cont_length = "CONTENT_LENGTH=" + oss.str();
    std::string cont_type = "CONTENT_TYPE=" + request.getHeader("content-type");

    char* env[] = {const_cast<char*>(method.c_str()),
                   const_cast<char*>(query.c_str()),
                   const_cast<char*>(script.c_str()),
                   const_cast<char*>(path_info.c_str()),
                   const_cast<char*>(cont_length.c_str()),
                   const_cast<char*>(cont_type.c_str()),
                   NULL};
    char* argv[] = {const_cast<char*>(loc.cgi_path.c_str()),
                    const_cast<char*>(script_name.c_str()),
                    NULL};

    execve(loc.cgi_path.c_str(), argv, env);
    std::cerr << "[cgi] execve() failed: " << strerror(errno) << std::endl;
    exit(1);
}

CgiProcess spawnCgi(std::string const& uri,
                    HttpRequest const& request,
                    Location const& loc)
{
    int pipe_in[2];
    int pipe_out[2];
    pid_t pid;
    CgiProcess failed = {-1, -1, -1};

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
        runChild(pipe_in, pipe_out, uri, request, loc);

    close(pipe_in[0]);
    close(pipe_out[1]);

    CgiProcess proc = {pid, pipe_in[1], pipe_out[0]};
    return proc;
}
