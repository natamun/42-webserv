/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dufama <dufama@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 16:57:46 by dufama            #+#    #+#             */
/*   Updated: 2026/07/02 16:57:46 by dufama           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ConfigParser.hpp"
#include <netinet/in.h>
#include <cctype>
#include <cstddef>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

ConfigParser::ConfigParser(std::string const& filepath) : _pos(0)
{
	_tokenize(filepath);
}

ConfigParser::~ConfigParser() {}

void ConfigParser::_tokenize(std::string const& filepath)
{
	std::ifstream file(filepath.c_str());
	if (!file.is_open())
		throw std::runtime_error("cannot open config file: " + filepath);

	std::string content((std::istreambuf_iterator<char>(file)),
							std::istreambuf_iterator<char>());

	std::string current;
	for (size_t i = 0; i < content.size(); ++i)
	{
		char c = content[i];
		if (c == '{' || c == '}' || c == ';')
		{
			if (!current.empty())
			{
				_tokens.push_back(current);
				current.clear();
			}
			_tokens.push_back(std::string(1, c));
		}
		else if (std::isspace(c))
		{
			if (!current.empty())
			{
				_tokens.push_back(current);
				current.clear();
			}
		}
		else
		{
			current += c;
		}
	}
	if (!current.empty())
		_tokens.push_back(current);
}

std::string const& ConfigParser::_peek() const
{
	if (_isEnd())
		throw std::runtime_error("unexpected end of config file");
	return _tokens[_pos];
}

std::string ConfigParser::_next()
{
	if (_isEnd())
		throw std::runtime_error("unexpected end of config file");
	return _tokens[_pos++];
}

void ConfigParser::_expect(std::string const& token)
{
	if (_next() != token)
		throw std::runtime_error("excpected '" + token + "' in config file");
}

bool ConfigParser::_isEnd() const
{
	return _pos >= _tokens.size();
}


std::vector<Server> ConfigParser::parse()
{
	std::vector<Server> servers;
	while (!_isEnd())
	{
		_expect("server");
		_expect("{");
		servers.push_back(_parseServer());
	}
	if (servers.empty())
		throw std::runtime_error("no server block found in config");
	return servers;
}

Server ConfigParser::_parseServer()
{
	Server server;

	while (_peek() != "}")
	{
		std::string directive = _next();

		if (directive == "listen")
		{
			server.port = std::atoi(_next().c_str());
			_expect(";");
		}
		else if (directive == "host")
		{
			server.host = _next();
			_expect(";");
		}
		else if (directive == "server_name")
		{
			server.server_name = _next();
			_expect(";");
		}
		else if (directive == "root")
		{
			server.root = _next();
			_expect(";");
		}
		else if (directive == "client_max_body_size")
		{
			server.client_max_body_size = std::atoi(_next().c_str());
			_expect(";");
		}
		else if (directive == "error_page")
		{
			int code = std::atoi(_next().c_str());
			std::string path = _next();
			server.error_pages[code] = path;
			_expect(";");
		}
		else if (directive == "location")
		{
			std::string path = _next();
			_expect("{");
			Location loc = _parseLocation();
			loc.path = path;
			server.locations.push_back(loc);
		}
		else
		{
			throw std::runtime_error("unknow directive :" + directive);
		}
	}
	_expect("}");

	for (size_t i = 0; i < server.locations.size(); ++i)
	{
		if (server.locations[i].root.empty())
			server.locations[i].root = server.root;
	}
	return server;
}

Location ConfigParser::_parseLocation()
{
	Location loc;
	loc.autoindex = false;

	while (_peek() != "}")
	{
		std::string directive = _next();

		if (directive == "allow_methods")
		{
			while (_peek() != ";")
				loc.methods.push_back(_next());
			_expect(";");
		}
		else if (directive == "root")
		{
			loc.root = _next();
			_expect(";");
		}
		else if (directive == "index")
		{
			loc.index = _next();
			_expect(";");
		}
		else if (directive == "autoindex")
		{
			loc.autoindex = (_next() == "on");
			_expect(";");
		}
		else if (directive == "upload_path")
		{
			loc.upload_path = _next();
			_expect(";");
		}
		else if (directive == "return")
		{
			loc.redirect = _next();
			_expect(";");
		}
		else
		{
			throw std::runtime_error("unknown location directive" + directive);
		}
	}
	_expect("}");
	return loc;
}