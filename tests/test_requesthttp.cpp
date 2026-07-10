/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_requesthttp.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/04 17:48:12 by dufama            #+#    #+#             */
/*   Updated: 2026/07/10 02:23:02 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// tests/test_httprequest.cpp
#include <cassert>
#include <iostream>
#include <sstream>

#include "HttpRequest.hpp"

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

// ---- Test 1 : requête GET simple, complète en un seul appendData ----
void test_simpleGet()
{
    TEST("simple GET request");
    HttpRequest req;
    std::string raw = "GET /index.html HTTP/1.1\r\nHost: localhost\r\n\r\n";
    req.appendData(raw.c_str(), raw.size());

    ASSERT_TRUE(req.isComplete());
    ASSERT_TRUE(!req.hasError());
    ASSERT_TRUE(req.getMethod() == "GET");
    ASSERT_TRUE(req.getUri() == "/index.html");
    ASSERT_TRUE(req.getVersion() == "HTTP/1.1");
    ASSERT_TRUE(req.getHeader("Host") == "localhost");
    ASSERT_TRUE(req.getBody().empty());
    PASS();
}

// ---- Test 2 : requête reçue en PLUSIEURS morceaux (simulateur de recv()
// partiel) ----
void test_fragmentedRequest()
{
    TEST("fragmented request across multiple appendData calls");
    HttpRequest req;

    // On coupe volontairement la requête en 3 morceaux
    std::string part1 = "GET /ind";
    std::string part2 = "ex.html HTTP/1.1\r\nHost: loc";
    std::string part3 = "alhost\r\n\r\n";

    req.appendData(part1.c_str(), part1.size());
    ASSERT_TRUE(!req.isComplete());  // pas encore complet après le 1er morceau

    req.appendData(part2.c_str(), part2.size());
    ASSERT_TRUE(!req.isComplete());  // toujours pas complet

    req.appendData(part3.c_str(), part3.size());
    ASSERT_TRUE(req.isComplete());  // complet après le 3e morceau

    ASSERT_TRUE(req.getMethod() == "GET");
    ASSERT_TRUE(req.getUri() == "/index.html");
    ASSERT_TRUE(req.getHeader("Host") == "localhost");
    PASS();
}

// ---- Test 3 : POST avec body et Content-Length ----
void test_postWithBody()
{
    TEST("POST with Content-Length body");
    HttpRequest req;
    std::string body = "{\"name\":\"test\"}";
    std::ostringstream oss;
    oss << "POST /upload HTTP/1.1\r\n"
        << "Content-Length: " << body.size() << "\r\n"
        << "\r\n"
        << body;
    std::string raw = oss.str();

    req.appendData(raw.c_str(), raw.size());

    ASSERT_TRUE(req.isComplete());
    ASSERT_TRUE(req.getMethod() == "POST");
    ASSERT_TRUE(req.getBody() == body);
    PASS();
}

// ---- Test 4 : body reçu en plusieurs morceaux (le body coupe en plein milieu)
// ----
void test_fragmentedBody()
{
    TEST("body split across multiple appendData calls");
    HttpRequest req;
    std::string headers = "POST /upload HTTP/1.1\r\nContent-Length: 10\r\n\r\n";
    std::string bodyPart1 = "12345";
    std::string bodyPart2 = "67890";

    req.appendData(headers.c_str(), headers.size());
    ASSERT_TRUE(!req.isComplete());  // headers OK mais body pas encore reçu

    req.appendData(bodyPart1.c_str(), bodyPart1.size());
    ASSERT_TRUE(!req.isComplete());  // body partiel

    req.appendData(bodyPart2.c_str(), bodyPart2.size());
    ASSERT_TRUE(req.isComplete());
    ASSERT_TRUE(req.getBody() == "1234567890");
    PASS();
}

// ---- Test 5 : requête malformée (request line invalide) ----
void test_malformedRequestLine()
{
    TEST("malformed request line triggers error state");
    HttpRequest req;
    std::string raw =
        "GET\r\nHost: localhost\r\n\r\n";  // il manque URI et VERSION

    req.appendData(raw.c_str(), raw.size());

    ASSERT_TRUE(req.hasError());
    ASSERT_TRUE(!req.isComplete());
    PASS();
}

// ---- Test 6 : header sans ':' -> erreur ----
void test_malformedHeader()
{
    TEST("header without colon triggers error state");
    HttpRequest req;
    std::string raw = "GET / HTTP/1.1\r\nBadHeaderNoColon\r\n\r\n";

    req.appendData(raw.c_str(), raw.size());

    ASSERT_TRUE(req.hasError());
    PASS();
}

// ---- Test 7 : header inexistant renvoie une string vide ----
void test_missingHeaderReturnsEmpty()
{
    TEST("getHeader on missing header returns empty string");
    HttpRequest req;
    std::string raw = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
    req.appendData(raw.c_str(), raw.size());

    ASSERT_TRUE(req.getHeader("X-Not-Present").empty());
    PASS();
}

// ---- Test 8 : plusieurs headers différents ----
void test_multipleHeaders()
{
    TEST("multiple distinct headers parsed correctly");
    HttpRequest req;
    std::string raw =
        "GET / HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "User-Agent: test-client\r\n"
        "Accept: text/html\r\n"
        "\r\n";
    req.appendData(raw.c_str(), raw.size());

    ASSERT_TRUE(req.isComplete());
    ASSERT_TRUE(req.getHeader("Host") == "localhost");
    ASSERT_TRUE(req.getHeader("User-Agent") == "test-client");
    ASSERT_TRUE(req.getHeader("Accept") == "text/html");
    PASS();
}

// ---- Test 9 : espaces multiples après ':' dans un header ----
void test_headerWithExtraSpaces()
{
    TEST("header value with leading spaces is trimmed");
    HttpRequest req;
    std::string raw = "GET / HTTP/1.1\r\nHost:    localhost\r\n\r\n";
    req.appendData(raw.c_str(), raw.size());

    ASSERT_TRUE(req.getHeader("Host") == "localhost");
    PASS();
}

// ---- Test 10 : deux requêtes GET consécutives, données une seule fois d'un
// coup ----
void test_getWithoutBody()
{
    TEST("GET without Content-Length completes without waiting for body");
    HttpRequest req;
    std::string raw = "GET /page HTTP/1.1\r\nHost: x\r\n\r\n";
    req.appendData(raw.c_str(), raw.size());

    ASSERT_TRUE(req.isComplete());
    ASSERT_TRUE(req.getBody().empty());
    PASS();
}

// ---- Test 11 : POST chunked complet en un seul appendData ----
void test_chunkedBody()
{
    TEST("POST with Transfer-Encoding: chunked decoded correctly");
    HttpRequest req;
    std::string raw =
        "POST /upload HTTP/1.1\r\n"
        "Transfer-Encoding: chunked\r\n"
        "\r\n"
        "5\r\n"
        "hello\r\n"
        "6\r\n"
        " world\r\n"
        "0\r\n"
        "\r\n";
    req.appendData(raw.c_str(), raw.size());

    ASSERT_TRUE(req.isComplete());
    ASSERT_TRUE(!req.hasError());
    ASSERT_TRUE(req.getBody() == "hello world");
    PASS();
}

// ---- Test 12 : chunked fragmenté en plusieurs appendData ----
void test_chunkedBodyFragmented()
{
    TEST("chunked body split across multiple appendData calls");
    HttpRequest req;

    std::string part1 =
        "POST /upload HTTP/1.1\r\n"
        "Transfer-Encoding: chunked\r\n"
        "\r\n"
        "5\r\n";
    std::string part2 = "hello\r\n";
    std::string part3 = "0\r\n\r\n";

    req.appendData(part1.c_str(), part1.size());
    ASSERT_TRUE(!req.isComplete());

    req.appendData(part2.c_str(), part2.size());
    ASSERT_TRUE(!req.isComplete());

    req.appendData(part3.c_str(), part3.size());
    ASSERT_TRUE(req.isComplete());
    ASSERT_TRUE(req.getBody() == "hello");
    PASS();
}

int main()
{
    test_simpleGet();
    test_fragmentedRequest();
    test_postWithBody();
    test_fragmentedBody();
    test_malformedRequestLine();
    test_malformedHeader();
    test_missingHeaderReturnsEmpty();
    test_multipleHeaders();
    test_headerWithExtraSpaces();
    test_getWithoutBody();
    test_chunkedBody();
    test_chunkedBodyFragmented();

    std::cout << "\n"
              << g_testsPassed << "/" << g_testsRun << " tests passed."
              << std::endl;

    return (g_testsPassed == g_testsRun) ? 0 : 1;
}