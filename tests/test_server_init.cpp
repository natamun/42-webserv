/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_server_init.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 01:52:32 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/02 04:25:37 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

#include "WebServer.hpp"
#include "WebServerException.hpp"

int main(void)
{
    Location loc1;
    loc1.path = "/";
    loc1.root = "./www";
    loc1.index = "index.html";
    loc1.autoindex = false;

    Location loc2;
    loc2.path = "/upload";
    loc2.root = "./www/upload";
    loc2.autoindex = true;
    loc2.upload_path = "./www/upload";
    loc2.methods.push_back("GET");
    loc2.methods.push_back("POST");

    Server srv;
    srv.host = "127.0.0.1";
    srv.port = 8080;
    srv.client_max_body_size = 1048576;
    srv.error_pages[404] = "./www/errors/404.html";
    srv.error_pages[500] = "./www/errors/500.html";
    srv.locations.push_back(loc1);
    srv.locations.push_back(loc2);

    Server srv_dup;
    srv_dup.host = "127.0.0.1";
    srv_dup.port = 8080;

    std::vector<Server> servers;
    servers.push_back(srv);

    std::cout << "=== Test 1: valid config ===\n\n";
    try
    {
        WebServer ws(servers);
        std::cout << ws << std::endl;
    }
    catch (WebServerException const& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    std::cout << "=== Test 2: duplicate port ===\n\n";
    servers.push_back(srv_dup);
    try
    {
        WebServer ws(servers);
        return 1;
    }
    catch (WebServerException const& e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
