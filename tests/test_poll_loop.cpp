/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_poll_loop.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 19:05:15 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/02 19:55:11 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

#include "WebServer.hpp"
#include "WebServerException.hpp"

// In another terminal juste launch telnet 127.0.0.1 8080 and type some text, it
// should print the msg
int main(void)
{
    Server srv;
    srv.host = "127.0.0.1";
    srv.port = 8080;
    srv.client_max_body_size = 1048576;

    std::vector<Server> servers;
    servers.push_back(srv);

    try
    {
        WebServer ws(servers);
        std::cout << "Listening on 127.0.0.1:8080 ..." << std::endl;
        ws.run();
    }
    catch (WebServerException const& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
