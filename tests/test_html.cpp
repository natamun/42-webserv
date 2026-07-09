/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_html.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dufama <dufama@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 17:29:04 by dufama            #+#    #+#             */
/*   Updated: 2026/07/08 17:29:04 by dufama           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <vector>

#include "ConfigParser.hpp"
#include "Server.hpp"
#include "WebServer.hpp"

int main()
{
    ConfigParser test("tests/.conf");
    std::vector<Server> server = test.parse();
    WebServer ws(server);
    ws.run();
}