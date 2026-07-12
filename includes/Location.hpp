/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 00:35:46 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/10 03:24:10 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <map>
#include <set>
#include <string>

class Location
{
public:
    std::string path;
    std::set<std::string> methods;
    std::string root;
    std::string index;
    bool autoindex;
    std::string upload_path;
    std::string redirect;
    std::map<std::string, std::string> cgi;
};

#endif