/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_cgi.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 00:59:05 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/09 01:15:05 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <csignal>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "ConfigParser.hpp"
#include "Server.hpp"
#include "WebServer.hpp"

int main()
{
    std::string config_path = "tests/cgi-test.conf";

    try
    {
        ConfigParser parser(config_path);
        std::vector<Server> servers = parser.parse();

        WebServer server(servers);
        std::cout << server << std::endl;
        server.run();
    }
    catch (std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
