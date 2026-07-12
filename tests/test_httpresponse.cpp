/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_httpresponse.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmunari <nmunari@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/12 18:13:32 by nmunari           #+#    #+#             */
/*   Updated: 2026/07/12 18:14:41 by nmunari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cassert>
#include <iostream>
#include <sstream>

#include "HttpResponse.hpp"

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

// ---- Test 1 : valeurs par defaut du constructeur ----
void test_defaultConstructor()
{
    TEST("default constructor: status 200, Connection: close");
    HttpResponse res;

    ASSERT_TRUE(res.getStatus() == 200);
    ASSERT_TRUE(res.build().find("HTTP/1.1 200 OK\r\n") == 0);
    ASSERT_TRUE(res.build().find("Connection: close\r\n") != std::string::npos);
    PASS();
}

// ---- Test 2 : setStatus / getStatus ----
void test_setStatusGetStatus()
{
    TEST("setStatus/getStatus round trip");
    HttpResponse res;
    res.setStatus(404);

    ASSERT_TRUE(res.getStatus() == 404);
    ASSERT_TRUE(res.build().find("HTTP/1.1 404 Not Found\r\n") == 0);
    PASS();
}

// ---- Test 3 : setBody met a jour Content-Length automatiquement ----
void test_setBodyUpdatesContentLength()
{
    TEST("setBody sets Content-Length header");
    HttpResponse res;
    res.setBody("hello world");

    std::string built = res.build();
    ASSERT_TRUE(built.find("Content-Length: 11\r\n") != std::string::npos);
    ASSERT_TRUE(built.find("hello world") != std::string::npos);
    PASS();
}

// ---- Test 4 : sans setBody, Content-Length par defaut a 0 ----
void test_defaultContentLengthIsZero()
{
    TEST("build() defaults Content-Length to 0 without setBody");
    HttpResponse res;

    ASSERT_TRUE(res.build().find("Content-Length: 0\r\n") != std::string::npos);
    PASS();
}

// ---- Test 5 : setHeader ajoute bien un header custom ----
void test_setHeaderCustom()
{
    TEST("setHeader adds a custom header to build() output");
    HttpResponse res;
    res.setHeader("X-Test", "value");

    ASSERT_TRUE(res.build().find("X-Test: value\r\n") != std::string::npos);
    PASS();
}

// ---- Test 6 : buildDefaultErrorBody genere le bon body/type ----
void test_buildDefaultErrorBody()
{
    TEST("buildDefaultErrorBody sets HTML body and Content-Type");
    HttpResponse res;
    res.setStatus(403);
    res.buildDefaultErrorBody();

    std::string built = res.build();
    ASSERT_TRUE(built.find("<h1>403 Forbidden</h1>") != std::string::npos);
    ASSERT_TRUE(built.find("Content-Type: text/html\r\n") != std::string::npos);
    PASS();
}

// ---- Test 7 : code de statut inconnu -> "Unknown" ----
void test_unknownStatusMessage()
{
    TEST("unknown status code falls back to 'Unknown'");
    HttpResponse res;
    res.setStatus(499);

    ASSERT_TRUE(res.build().find("HTTP/1.1 499 Unknown\r\n") == 0);
    PASS();
}

// ---- Test 8 : version HTTP personnalisee ----
void test_setVersion()
{
    TEST("setVersion changes the status line");
    HttpResponse res;
    res.setVersion("HTTP/1.0");

    ASSERT_TRUE(res.build().find("HTTP/1.0 200 OK\r\n") == 0);
    PASS();
}

// ---- Test 9 : constructeur de copie ----
void test_copyConstructor()
{
    TEST("copy constructor produces identical build() output");
    HttpResponse original;
    original.setStatus(201);
    original.setBody("copied");

    HttpResponse copy(original);

    ASSERT_TRUE(copy.build() == original.build());
    PASS();
}

// ---- Test 10 : operator= ----
void test_assignmentOperator()
{
    TEST("operator= produces identical build() output");
    HttpResponse a;
    a.setStatus(500);
    a.setBody("error");

    HttpResponse b;
    b = a;

    ASSERT_TRUE(b.build() == a.build());
    PASS();
}

int main()
{
    test_defaultConstructor();
    test_setStatusGetStatus();
    test_setBodyUpdatesContentLength();
    test_defaultContentLengthIsZero();
    test_setHeaderCustom();
    test_buildDefaultErrorBody();
    test_unknownStatusMessage();
    test_setVersion();
    test_copyConstructor();
    test_assignmentOperator();

    std::cout << "\n"
              << g_testsPassed << "/" << g_testsRun << " tests passed."
              << std::endl;

    return (g_testsPassed == g_testsRun) ? 0 : 1;
}
