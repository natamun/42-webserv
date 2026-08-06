/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/30 19:40:15 by nmunari           #+#    #+#             */
/*   Updated: 2026/08/04 17:34:18 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <csignal>
#include <exception>
#include <iostream>
#include <vector>

#include "ConfigParser.hpp"
#include "Server.hpp"
#include "WebServer.hpp"

volatile bool g_running = true;

static void sigHandler(int)
{
    g_running = false;
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return 1;
    }

    signal(SIGINT, sigHandler);
    signal(SIGTERM, sigHandler);

    try
    {
        ConfigParser parser(argv[1]);
        std::vector<Server> servers = parser.parse();
        WebServer server(servers);
        server.run();
    }
    catch (std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
