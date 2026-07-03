/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dufama <dufama@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 16:52:29 by dufama            #+#    #+#             */
/*   Updated: 2026/07/02 16:52:29 by dufama           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <cstddef>
#include <string>
#include <vector>

#include "Location.hpp"
#include "Server.hpp"

class ConfigParser
{
public:
    ConfigParser(std::string const& filepath);
    ~ConfigParser();

    std::vector<Server> parse();

private:
    ConfigParser(ConfigParser const& copy);
    ConfigParser& operator=(ConfigParser const& other);

    std::vector<std::string> _tokens;
    std::size_t _pos;

    void _tokenize(std::string const& filpath);
    Server _parseServer();
    Location _parseLocation();

    std::string const& _peek() const;
    std::string _next();
    void _expect(std::string const& token);
    bool _isEnd() const;
};

#endif