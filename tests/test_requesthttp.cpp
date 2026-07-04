/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_requesthttp.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dufama <dufama@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/04 17:48:12 by dufama            #+#    #+#             */
/*   Updated: 2026/07/04 17:48:12 by dufama           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

#include "HttpRequest.hpp"

int main()
{
    HttpRequest req;
    std::string raw = "GET /index.html HTTP/1.1\r\n";
    req.appendData(raw.c_str(), raw.size());
    std::cout << req.getBody() << std::endl;
    std::cout << req.getMehtod() << std::endl;
}