/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_cgi.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 00:59:05 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/10 02:15:34 by nmunari          ###   ########.fr       */
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
    // Testing POST with QUERY
    // curl -X POST -d "name=hello&value=world"
    // "http://localhost:8080/cgi-bin/test.py?foo=bar&age=42"

    // Testing GET with QUERY
    // curl "http://localhost:8080/cgi-bin/test.py?name=hello&age=42"
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
