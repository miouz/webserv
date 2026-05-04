#include "../../src/RequestParser.hpp"
#include "../testUtils.hpp"
#include <iostream>
#include <string>

// Local Helper
static RequestParser make_parser(std::string raw)
{
    RequestParser p;
    p.feed(raw);
    return p;
}

// ═════════════════════════════════════════════════════════════════════════════
//  1. Basic methods
// ═════════════════════════════════════════════════════════════════════════════

static void test_simple_get(void)
{
    SUITE("Simple GET");
    RequestParser p = make_parser("GET / HTTP/1.0\r\nHost: localhost\r\n\r\n");
    ASSERT_EQ("complete",   true,       p.isComplete());
    ASSERT_EQ("method",     std::string("GET"),      p.getData().method);
    ASSERT_EQ("uri",        std::string("/"),         p.getData().uri);
    ASSERT_EQ("protocol",   std::string("HTTP/1.0"), p.getData().protocol);
	ASSERT_EQ("code", 200, p.getData().code);
    SUITE_END();
}

static void test_leading_space_post(void)
{
    SUITE("POST with space");
    RequestParser p = make_parser(" POST / HTTP/1.0\r\nHost: localhost\r\n\r\n");
    ASSERT_EQ("complete",  true,        p.isComplete());
    ASSERT_EQ("method",    std::string("POST"),     p.getData().method);
    ASSERT_EQ("uri",       std::string("/"),        p.getData().uri);
    ASSERT_EQ("protocol",  std::string("HTTP/1.0"), p.getData().protocol);
	ASSERT_EQ("code", 200, p.getData().code);
    SUITE_END();
}

static void test_leading_space_delete(void)
{
    SUITE("DELETE with space");
    RequestParser p = make_parser(" DELETE / HTTP/1.0\r\nHost: localhost\r\n\r\n");
    ASSERT_EQ("complete",  true,        p.isComplete());
    ASSERT_EQ("method",    std::string("DELETE"),   p.getData().method);
    ASSERT_EQ("uri",       std::string("/"),        p.getData().uri);
    ASSERT_EQ("protocol",  std::string("HTTP/1.0"), p.getData().protocol);
	ASSERT_EQ("code", 200, p.getData().code);
    SUITE_END();
}

static void test_missing_protocol(void)
{
    SUITE("GET — protocol missing");
    RequestParser p = make_parser("GET /\r\nHost: localhost\r\n\r\n");
    ASSERT_EQ("complete",  true,        p.isComplete());
    ASSERT_EQ("method",    std::string("GET"),   p.getData().method);
    ASSERT_EQ("uri",       std::string("/"),        p.getData().uri);
    ASSERT_EQ("protocol",  std::string(""), p.getData().protocol);
	ASSERT_EQ("code", 400, p.getData().code);
    SUITE_END();
}

static void test_missing_host_header(void)
{
    SUITE("header Host missing (HTTP/1.0)");
	RequestParser p = make_parser("GET / HTTP/1.0\r\n\r\n");
    ASSERT_EQ("complete",  true,        p.isComplete());
    ASSERT_EQ("method",    std::string("GET"),   p.getData().method);
    ASSERT_EQ("uri",       std::string("/"),        p.getData().uri);
    ASSERT_EQ("protocol",  std::string("HTTP/1.0"), p.getData().protocol);
	ASSERT_EQ("code", 200, p.getData().code);
    SUITE_END();
}


// ═════════════════════════════════════════════════════════════════════════════
//  2. Multiple headers
// ═════════════════════════════════════════════════════════════════════════════

static void test_multiple_headers(void)
{
    SUITE("multiples Headers");
    std::string raw =
        "GET /page HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "Accept: text/html\r\n"
        "Connection: keep-alive\r\n"
        "User-Agent: webserv-test/1.0\r\n"
        "\r\n";
    RequestParser p = make_parser(raw);
    ASSERT_EQ("complete",               true,                       p.isComplete());
    ASSERT_EQ("Host header",            std::string(" example.com"),       p.getData().headers.at("HOST"));
    ASSERT_EQ("Accept header",          std::string(" text/html"),          p.getData().headers.at("ACCEPT"));
    ASSERT_EQ("Connection header",      std::string(" keep-alive"),         p.getData().headers.at("CONNECTION"));
    ASSERT_EQ("User-Agent header",      std::string(" webserv-test/1.0"),   p.getData().headers.at("USER-AGENT"));
	ASSERT_EQ("code", 200, p.getData().code);
    SUITE_END();
}

static void test_header_case_insensitive(void)
{
    SUITE("Headers — unsensitive case ");
    std::string raw =
        "GET / HTTP/1.1\r\n"
        "HOST: localhost\r\n"
        "content-type: application/json\r\n"
        "\r\n";
    RequestParser p = make_parser(raw);
    ASSERT_EQ("complete",        true,                           p.isComplete());
    ASSERT_EQ("HOST normalized",  std::string(" localhost"),             p.getData().headers.at("HOST"));
    ASSERT_EQ("content-type",    std::string(" application/json"),      p.getData().headers.at("CONTENT-TYPE"));
	ASSERT_EQ("code", 200, p.getData().code);
    SUITE_END();
}

static void	test_random_header(void)
{
    SUITE("Headers — random header");
    std::string raw =
        "GET / HTTP/1.1\r\n"
        "garbage: garbage garbage\r\n"
        "\r\n";
    RequestParser p = make_parser(raw);
    ASSERT_EQ("complete",        true,                           p.isComplete());
    ASSERT_EQ("header garbage: garbage garbage",  std::string(" garbage garbage"),             p.getData().headers.at("GARBAGE"));
	ASSERT_EQ("code", 200, p.getData().code);
    SUITE_END();
}

static void	test_incomplete_header(void)
{
    SUITE("Headers — incomplete header");
    std::string raw =
        "GET / HTTP/1.1\r\n"
        "header:\r\n"
        "\r\n";
    RequestParser p = make_parser(raw);
    ASSERT_EQ("complete",        true,                           p.isComplete());
    ASSERT_EQ("header:",  std::string(""),             p.getData().headers.at("HEADER"));
	ASSERT_EQ("code", 200, p.getData().code);
    SUITE_END();
}

static void	test_garbage_host_header()
{
	SUITE("Headers — garbage host header");
    std::string raw =
        "GET / HTTP/1.1\r\n"
        "Host: garbage\r\n"
        "\r\n";
    RequestParser p = make_parser(raw);
    ASSERT_EQ("complete",        true,                           p.isComplete());
    ASSERT_EQ("Host: garbage",  std::string(" garbage"),             p.getData().headers.at("HOST"));
	ASSERT_EQ("code", 200, p.getData().code);
    SUITE_END();
}

// ═════════════════════════════════════════════════════════════════════════════
//  3. Complexe URI
// ═════════════════════════════════════════════════════════════════════════════

static void test_uri_with_query_string(void)
{
    SUITE("URI — query string");
    RequestParser p = make_parser("GET /search?q=hello&lang=fr HTTP/1.1\r\nHost: localhost\r\n\r\n");
    ASSERT_EQ("complete",  true,                                p.isComplete());
    ASSERT_EQ("uri",       std::string("/search?q=hello&lang=fr"),   p.getData().uri);
	ASSERT_EQ("code", 200, p.getData().code);
    SUITE_END();
}

static void test_uri_with_path_params(void)
{
    SUITE("URI — embedded path");
    RequestParser p = make_parser("GET /api/v1/users/42/profile HTTP/1.1\r\nHost: localhost\r\n\r\n");
    ASSERT_EQ("complete",  true,                               p.isComplete());
    ASSERT_EQ("uri",       std::string("/api/v1/users/42/profile"),  p.getData().uri);
	ASSERT_EQ("code", 200, p.getData().code);
    SUITE_END();
}

static void test_uri_with_fragment(void)
{
    SUITE("URI — fragment (#)");
    RequestParser p = make_parser("GET /page#section2 HTTP/1.1\r\nHost: localhost\r\n\r\n");
    ASSERT_EQ("complete",  true,                        p.isComplete());
    ASSERT_EQ("uri",       std::string("/page#section2"),    p.getData().uri);
	ASSERT_EQ("code", 200, p.getData().code);
    SUITE_END();
}

static void test_uri_encoded(void)
{
    SUITE("URI — encoded char (%XX)");
    RequestParser p = make_parser("GET /path/hello%20world?name=fo%C3%B6 HTTP/1.1\r\nHost: localhost\r\n\r\n");
    ASSERT_EQ("complete",  true,                                       p.isComplete());
    ASSERT_EQ("uri",       std::string("/path/hello%20world?name=fo%C3%B6"), p.getData().uri);
	ASSERT_EQ("code", 200, p.getData().code);
    SUITE_END();
}

// ═════════════════════════════════════════════════════════════════════════════
//  4. Body / Content-Length
// ═════════════════════════════════════════════════════════════════════════════

static void test_post_with_body(void)
{
    SUITE("POST with body");
    std::string raw =
        "POST /submit HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Length: 13\r\n"
        "\r\n"
        "hello, world!";
    RequestParser p = make_parser(raw);
    ASSERT_EQ("complete",        true,                p.isComplete());
    ASSERT_EQ("method",          std::string("POST"),      p.getData().method);
    ASSERT_EQ("body",            std::string("hello, world!"), p.getData().body);
    ASSERT_EQ("Content-Length",  std::string(" 13"),       p.getData().headers.at("CONTENT-LENGTH"));
	ASSERT_EQ("code", 200, p.getData().code);
    SUITE_END();
}

static void test_post_body_truncated(void)
{
    SUITE("POST body truncated (Content-Length > body)");
    std::string raw =
        "POST /upload HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Length: 100\r\n"
        "\r\n"
        "short";
    RequestParser p = make_parser(raw);
    ASSERT_INVALID("not complete if insuffisant body", p);
	ASSERT_EQ("code", 200, p.getData().code);
    SUITE_END();
}

static void test_post_empty_body(void)
{
    SUITE("POST empty body (Content-Length: 0)");
    std::string raw =
        "POST /empty HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Length: 0\r\n"
        "\r\n";
    RequestParser p = make_parser(raw);
    ASSERT_EQ("complete",  true,         p.isComplete());
    ASSERT_EQ("empty body", std::string(""), p.getData().body);
	ASSERT_EQ("code", 200, p.getData().code);
    SUITE_END();
}

// ═════════════════════════════════════════════════════════════════════════════
//  5. Bad input / errors
// ═════════════════════════════════════════════════════════════════════════════

static void test_unknown_method(void)
{
    SUITE("Bad input — unknown method");
    RequestParser p = make_parser("INVALID / HTTP/1.1\r\nHost: localhost\r\n\r\n");
	ASSERT_EQ("complete", true, p.isComplete());
	ASSERT_EQ("code", 400, p.getData().code);
    SUITE_END();
}

static void test_double_crlf_only(void)
{
    SUITE("Bad input — empty request");
    RequestParser p = make_parser("\r\n\r\n");
	ASSERT_EQ("complete", true, p.isComplete());
	ASSERT_EQ("code", 400, p.getData().code);
    SUITE_END();
}

// static void test_invalid_http_version(void)
// {
//     SUITE("Bad input — invalid version HTTP");
//     bool threw = false;
//     try {
//         RequestParser p = make_parser("GET / HTTP/9.9\r\nHost: localhost\r\n\r\n");
//         ASSERT_INVALID("rejected if unknown version", p);
//     } catch (std::exception& e) {
//         threw = true;
//         std::cout << C_GREEN "  ✓ " C_RESET "exception rised : " << e.what() << "\n";
//         g_pass++;
//     }
//     (void)threw;
//     SUITE_END();
// }

static void	test_multiple_host_header(void)
{
    SUITE("Bad input — multiple host header");
    RequestParser p = make_parser("GET / HTTP/1.1\r\nHost: localhost\r\nHost: blabla\r\n\r\n");
	ASSERT_EQ("complete", true, p.isComplete());
	ASSERT_EQ("code", 400, p.getData().code);
    SUITE_END();
}

static void	test_multiple_content_length_header(void)
{
    SUITE("Bad input — multiple Content-Length header");
    RequestParser p = make_parser("GET / HTTP/1.1\r\nContent-Length: 2\r\nContent-Length: 3\r\n\r\n");
    ASSERT_EQ("complete",  true,         p.isComplete());
    ASSERT_EQ("code", 400, p.getData().code);
    SUITE_END();
}

static void	test_garbage_content_length_header()
{
	SUITE("Headers — garbage Content-Length header");
    std::string raw =
        "GET / HTTP/1.1\r\n"
        "Content-Length: garbage\r\n"
        "\r\n";
    RequestParser p = make_parser(raw);
    ASSERT_EQ("complete",        true,                           p.isComplete());
    ASSERT_EQ("Content-Length: garbage",  std::string(" garbage"),             p.getData().headers.at("CONTENT-LENGTH"));
	ASSERT_EQ("code", 400, p.getData().code);
    SUITE_END();
}

// ═════════════════════════════════════════════════════════════════════════════
//  Entry
// ═════════════════════════════════════════════════════════════════════════════

void run_http_parser_tests(void)
{
    std::cout << C_BOLD C_CYAN
              << "\n╔══════════════════════════════════════╗\n"
              << "║       HTTP REQUEST PARSER TESTS      ║\n"
              << "╚══════════════════════════════════════╝"
              << C_RESET "\n";

    test_simple_get();
    test_leading_space_post();
    test_leading_space_delete();
    test_missing_protocol();

    // Headers
    test_missing_host_header();
    test_multiple_headers();
    test_header_case_insensitive();
    test_random_header();
    test_incomplete_header();
    test_garbage_host_header();

    // URI
    test_uri_with_query_string();
    test_uri_with_path_params();
    test_uri_with_fragment();
    test_uri_encoded();

    // Body
    test_post_with_body();
    test_post_body_truncated();
    test_post_empty_body();

    // Bad input
    test_unknown_method();
    test_double_crlf_only();
    // test_invalid_http_version();
    test_multiple_host_header();
    test_multiple_content_length_header();
    test_garbage_content_length_header();
}
