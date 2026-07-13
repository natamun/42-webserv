/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_configparser.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/12 18:12:43 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/12 18:14:46 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include "ConfigParser.hpp"
#include "Location.hpp"
#include "Server.hpp"

static int g_testsRun = 0;
static int g_testsPassed = 0;

#define TEST(name)                                   \
    do                                               \
    {                                                \
        std::cout << "Running: " << name << " ... "; \
        g_testsRun++;                                \
    } while (0)

#define ASSERT_TRUE(cond)                                              \
    do                                                                 \
    {                                                                  \
        if (!(cond))                                                   \
        {                                                              \
            std::cout << "FAILED (line " << __LINE__ << ": " #cond ")" \
                      << std::endl;                                    \
            return;                                                    \
        }                                                              \
    } while (0)

#define PASS()                          \
    do                                  \
    {                                   \
        std::cout << "OK" << std::endl; \
        g_testsPassed++;                \
    } while (0)

static std::string const TMP_PATH = "/tmp/webserv_test_configparser.conf";

static std::string writeTempConf(std::string const& content)
{
    std::ofstream out(TMP_PATH.c_str());
    out << content;
    out.close();
    return TMP_PATH;
}

static bool throwsRuntimeError(std::string const& conf)
{
    std::string path = writeTempConf(conf);
    bool threw = false;
    try
    {
        ConfigParser parser(path);
        parser.parse();
    }
    catch (std::runtime_error const&)
    {
        threw = true;
    }
    std::remove(path.c_str());
    return threw;
}

// ---- Test 1 : parsing complet de default.conf ----
void test_parseDefaultConf()
{
    TEST("parse default.conf produces the expected server");
    ConfigParser parser("default.conf");
    std::vector<Server> servers = parser.parse();

    ASSERT_TRUE(servers.size() == 1);
    Server const& srv = servers[0];

    ASSERT_TRUE(srv.port == 8080);
    ASSERT_TRUE(srv.host == "127.0.0.1");
    ASSERT_TRUE(srv.server_name == "localhost");
    ASSERT_TRUE(srv.root == "www");
    ASSERT_TRUE(srv.client_max_body_size == 10000000);

    std::map<int, std::string>::const_iterator it404 =
        srv.error_pages.find(404);
    ASSERT_TRUE(it404 != srv.error_pages.end());
    ASSERT_TRUE(it404->second == "www/errors/404.html");

    ASSERT_TRUE(srv.locations.size() == 5);
    ASSERT_TRUE(srv.locations[0].path == "/");
    ASSERT_TRUE(srv.locations[0].methods.count("GET") == 1);
    ASSERT_TRUE(srv.locations[0].index == "index.html");
    ASSERT_TRUE(srv.locations[0].autoindex == false);

    ASSERT_TRUE(srv.locations[3].path == "/upload");
    ASSERT_TRUE(srv.locations[3].methods.count("POST") == 1);
    ASSERT_TRUE(srv.locations[3].methods.count("DELETE") == 1);
    ASSERT_TRUE(srv.locations[3].upload_path == "www/upload");

    ASSERT_TRUE(srv.locations[4].path == "/cgi-bin");
    ASSERT_TRUE(srv.locations[4].cgi.count(".py") == 1);
    PASS();
}

// ---- Test 2 : plusieurs blocs server dans le meme fichier ----
void test_parseMultiPortConf()
{
    TEST("parse tests/multi-port.conf produces two distinct servers");
    ConfigParser parser("tests/multi-port.conf");
    std::vector<Server> servers = parser.parse();

    ASSERT_TRUE(servers.size() == 2);
    ASSERT_TRUE(servers[0].port == 8080);
    ASSERT_TRUE(servers[0].root == "www");
    ASSERT_TRUE(servers[1].port == 8081);
    ASSERT_TRUE(servers[1].root == "www2");
    PASS();
}

// ---- Test 3 : fichier de config inexistant ----
void test_missingFileThrows()
{
    TEST("nonexistent config file throws");
    bool threw = false;
    try
    {
        ConfigParser parser("tests/does-not-exist.conf");
    }
    catch (std::runtime_error const&)
    {
        threw = true;
    }
    ASSERT_TRUE(threw);
    PASS();
}

// ---- Test 4 : port invalide (hors plage) ----
void test_invalidPortOutOfRangeThrows()
{
    TEST("out-of-range listen port throws");
    ASSERT_TRUE(
        throwsRuntimeError("server {\n"
                           "    listen 99999;\n"
                           "    host 127.0.0.1;\n"
                           "    root www;\n"
                           "    location / { allow_methods GET; }\n"
                           "}\n"));
    PASS();
}

// ---- Test 5 : port invalide (non numerique) ----
void test_invalidPortNotNumberThrows()
{
    TEST("non-numeric listen port throws");
    ASSERT_TRUE(
        throwsRuntimeError("server {\n"
                           "    listen abc;\n"
                           "    host 127.0.0.1;\n"
                           "    root www;\n"
                           "    location / { allow_methods GET; }\n"
                           "}\n"));
    PASS();
}

// ---- Test 6 : location sans allow_methods ----
void test_locationWithoutMethodsThrows()
{
    TEST("location without allow_methods throws");
    ASSERT_TRUE(
        throwsRuntimeError("server {\n"
                           "    listen 8080;\n"
                           "    host 127.0.0.1;\n"
                           "    root www;\n"
                           "    location / { root www; }\n"
                           "}\n"));
    PASS();
}

// ---- Test 7 : location dupliquee dans le meme server ----
void test_duplicateLocationThrows()
{
    TEST("duplicate location path throws");
    ASSERT_TRUE(
        throwsRuntimeError("server {\n"
                           "    listen 8080;\n"
                           "    host 127.0.0.1;\n"
                           "    root www;\n"
                           "    location / { allow_methods GET; }\n"
                           "    location / { allow_methods POST; }\n"
                           "}\n"));
    PASS();
}

// ---- Test 8 : ni root sur le server ni sur la location ----
void test_missingRootThrows()
{
    TEST("location and server both without root throws");
    ASSERT_TRUE(
        throwsRuntimeError("server {\n"
                           "    listen 8080;\n"
                           "    host 127.0.0.1;\n"
                           "    location / { allow_methods GET; }\n"
                           "}\n"));
    PASS();
}

// ---- Test 9 : directive inconnue ----
void test_unknownDirectiveThrows()
{
    TEST("unknown top-level directive throws");
    ASSERT_TRUE(
        throwsRuntimeError("server {\n"
                           "    listen 8080;\n"
                           "    host 127.0.0.1;\n"
                           "    root www;\n"
                           "    banana yes;\n"
                           "    location / { allow_methods GET; }\n"
                           "}\n"));
    PASS();
}

// ---- Test 10 : fichier sans aucun bloc server ----
void test_emptyConfigThrows()
{
    TEST("config with no server block throws");
    ASSERT_TRUE(throwsRuntimeError("# just a comment, no server block\n"));
    PASS();
}

int main()
{
    test_parseDefaultConf();
    test_parseMultiPortConf();
    test_missingFileThrows();
    test_invalidPortOutOfRangeThrows();
    test_invalidPortNotNumberThrows();
    test_locationWithoutMethodsThrows();
    test_duplicateLocationThrows();
    test_missingRootThrows();
    test_unknownDirectiveThrows();
    test_emptyConfigThrows();

    std::cout << "\n"
              << g_testsPassed << "/" << g_testsRun << " tests passed."
              << std::endl;

    return (g_testsPassed == g_testsRun) ? 0 : 1;
}
