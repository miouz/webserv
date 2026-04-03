#include "../../src/Config.hpp"
#include "../testUtils.hpp"
#include <iostream>
#include <fstream>
#include <string>

// ─────────────────────────────────────────────────────────────────────────────
//  Local helpers
// ─────────────────────────────────────────────────────────────────────────────


// Returns true if constructing Config from content throws any exception.
static bool throws_on(const std::string& content)
{
    try { Config c(content); return false; }
    catch (...) {
		std::cout << content << " not found\n";
		return true; }
}

// ═════════════════════════════════════════════════════════════════════════════
//  1. Minimal / basic server block
// ═════════════════════════════════════════════════════════════════════════════

static void test_empty_server_block(void)
{
    SUITE("Minimal — empty server block");
    bool threw = throws_on("test/config-parser-test/minimal-empty-server-block.txt");
    ASSERT_EQ("parses without throw", false, threw);
    if (!threw)
    {
        Config c("test/config-parser-test/minimal-empty-server-block.txt");
        ASSERT_EQ("one server", (size_t)1, c.getServers().size());
    }
    SUITE_END();
}

static void test_single_listen(void)
{
    SUITE("Minimal — listen directive");
    bool threw = throws_on("test/config-parser-test/single-listen.txt");
    ASSERT_EQ("parses without throw", false, threw);
    if (!threw)
    {
		Config c("test/config-parser-test/single-listen.txt");
		ASSERT_EQ("server count",  (size_t)1,       c.getServers().size());
		ASSERT_EQ("listen value",  std::string("8080"), c.getServers()[0].getListen());
	}
    SUITE_END();
}

static void test_single_server_name(void)
{
    SUITE("Minimal — server_name directive");
    bool threw = throws_on("test/config-parser-test/single-server-name.txt");
    ASSERT_EQ("parses without throw", false, threw);
    if (!threw)
    {
		Config c("test/config-parser-test/single-server-name.txt");
		ASSERT_EQ("server_name", std::string("example.com"), c.getServers()[0].getServerName());
	}
    SUITE_END();
}

static void test_single_root(void)
{
    SUITE("Minimal — root directive");
    Config c("test/config-parser-test/single-root.txt");
    ASSERT_EQ("root", std::string("/var/www/html"), c.getServers()[0].getRoot());
    SUITE_END();
}

static void test_client_max_body_size(void)
{
    SUITE("Minimal — client_max_body_size");
    Config c("test/config-parser-test/client-max-body-size.txt");
    ASSERT_EQ("client_max_body_size", 4096, c.getServers()[0].getClientMaxBodySize());
    SUITE_END();
}

// ═════════════════════════════════════════════════════════════════════════════
//  2. Multiple servers
// ═════════════════════════════════════════════════════════════════════════════

static void test_two_servers(void)
{
    SUITE("Multiple servers — two blocks");
    Config c("test/config-parser-test/two-servers.txt");
    ASSERT_EQ("server count", (size_t)2, c.getServers().size());
    ASSERT_EQ("first listen",  std::string("8080"), c.getServers()[0].getListen());
    ASSERT_EQ("second listen", std::string("9090"), c.getServers()[1].getListen());
    SUITE_END();
}

static void test_three_servers(void)
{
    SUITE("Multiple servers — three blocks");
    Config c("test/config-parser-test/three-servers.txt");
    ASSERT_EQ("server count", (size_t)3, c.getServers().size());
    SUITE_END();
}

// ═════════════════════════════════════════════════════════════════════════════
//  3. Index directive
// ═════════════════════════════════════════════════════════════════════════════

static void test_single_index(void)
{
    SUITE("Index — single value");
    Config c("test/config-parser-test/single-index.txt");
    std::vector<std::string> idx = c.getServers()[0].getIndex();
    ASSERT_EQ("index count", (size_t)1, idx.size());
    ASSERT_EQ("index[0]", std::string("index.html"), idx[0]);
    SUITE_END();
}

static void test_multiple_index(void)
{
    SUITE("Index — multiple values");
    Config c("test/config-parser-test/multiple-index.txt");
    std::vector<std::string> idx = c.getServers()[0].getIndex();
    ASSERT_EQ("index count", (size_t)3, idx.size());
    ASSERT_EQ("index[0]", std::string("index.html"), idx[0]);
    ASSERT_EQ("index[1]", std::string("index.htm"),  idx[1]);
    ASSERT_EQ("index[2]", std::string("default.html"), idx[2]);
    SUITE_END();
}

// ═════════════════════════════════════════════════════════════════════════════
//  4. Error pages
// ═════════════════════════════════════════════════════════════════════════════

static void test_error_page_404(void)
{
    SUITE("Error page — 404");
    Config c("test/config-parser-test/error-page-404.txt");
    std::map<int, std::string> ep = c.getServers()[0].getErrorPage();
    ASSERT_EQ("error_page count", (size_t)1, ep.size());
    ASSERT_EQ("404 page", std::string("/errors/404.html"), ep[404]);
    SUITE_END();
}

static void test_multiple_error_pages(void)
{
    SUITE("Error page — multiple codes");
    Config c( "test/config-parser-test/multiple-error-pages.txt");
    std::map<int, std::string> ep = c.getServers()[0].getErrorPage();
    ASSERT_EQ("error_page count", (size_t)2, ep.size());
    ASSERT_EQ("404 page", std::string("/404.html"), ep[404]);
    ASSERT_EQ("500 page", std::string("/500.html"), ep[500]);
    SUITE_END();
}

// ═════════════════════════════════════════════════════════════════════════════
//  5. Location blocks
// ═════════════════════════════════════════════════════════════════════════════

static void test_empty_location(void)
{
    SUITE("Location — empty block");
    Config c("test/config-parser-test/empty-location.txt");
    std::vector<Location> locs = c.getServers()[0].getLocations();
    ASSERT_EQ("location count", (size_t)1, locs.size());
    ASSERT_EQ("location path", std::string("/"), locs[0].getPath());
    SUITE_END();
}

static void test_location_root(void)
{
    SUITE("Location — root directive");
    Config c("test/config-parser-test/location-root.txt");
    Location loc = c.getServers()[0].getLocations()[0];
    ASSERT_EQ("location root", std::string("/var/www"), loc.getRoot());
    SUITE_END();
}

static void test_location_methods_get_only(void)
{
    SUITE("Location — methods GET only");
    Config c("test/config-parser-test/location-methods-get-only.txt");
    bool* m = c.getServers()[0].getLocations()[0].getMethods();
    ASSERT_EQ("GET allowed",    true,  m[0]);
    ASSERT_EQ("POST denied",   false, m[1]);
    ASSERT_EQ("DELETE denied", false, m[2]);
    SUITE_END();
}

static void test_location_methods_all(void)
{
    SUITE("Location — methods GET POST DELETE");
    Config c("test/config-parser-test/location-methods-all.txt");
    bool* m = c.getServers()[0].getLocations()[0].getMethods();
    ASSERT_EQ("GET",    true, m[0]);
    ASSERT_EQ("POST",   true, m[1]);
    ASSERT_EQ("DELETE", true, m[2]);
    SUITE_END();
}

static void test_location_autoindex_on(void)
{
    SUITE("Location — autoindex on");
    Config c("test/config-parser-test/location-autoindex-on.txt");
    ASSERT_EQ("autoindex", true, c.getServers()[0].getLocations()[0].getAutoindex());
    SUITE_END();
}

static void test_location_autoindex_off(void)
{
    SUITE("Location — autoindex off");
    Config c("test/config-parser-test/location-autoindex-off.txt");
    ASSERT_EQ("autoindex", false, c.getServers()[0].getLocations()[0].getAutoindex());
    SUITE_END();
}

static void test_location_index(void)
{
    SUITE("Location — index directive");
    Config c("test/config-parser-test/location-index.txt");
    std::vector<std::string> idx = c.getServers()[0].getLocations()[0].getIndex();
    ASSERT_EQ("index count", (size_t)2, idx.size());
    ASSERT_EQ("index[0]", std::string("index.html"), idx[0]);
    ASSERT_EQ("index[1]", std::string("index.php"),  idx[1]);
    SUITE_END();
}

static void test_location_upload_store(void)
{
    SUITE("Location — upload_store");
    Config c("test/config-parser-test/location-upload-store.txt");
    ASSERT_EQ("upload_store", std::string("/tmp/uploads"),
        c.getServers()[0].getLocations()[0].getUploadStore());
    SUITE_END();
}

static void test_location_cgi_pass(void)
{
    SUITE("Location — cgi_path");
    Config c("test/config-parser-test/location-cgi-pass.txt");
    ASSERT_EQ("cgi_pass", std::string("/usr/bin/python3"),
        c.getServers()[0].getLocations()[0].getCgiPass());
    SUITE_END();
}

static void test_location_cgi_extension(void)
{
    SUITE("Location — cgi_extension");
    Config c("test/config-parser-test/location-cgi-extension.txt");
    std::map<std::string, std::string> ext = c.getServers()[0].getLocations()[0].getCgiExtension();
    ASSERT_EQ("extension count", (size_t)1, ext.size());
    ASSERT_EQ(".py handler", std::string("/usr/bin/python3"), ext[".py"]);
    SUITE_END();
}

static void test_location_return(void)
{
    SUITE("Location — return directive");
    Config c("test/config-parser-test/location-return.txt");
    std::map<int, std::string> ep = c.getServers()[0].getLocations()[0].getErrorPage();
    ASSERT_EQ("return count", (size_t)1, ep.size());
    ASSERT_EQ("301 target", std::string("/new"), ep[301]);
    SUITE_END();
}

static void test_location_client_max_body_size(void)
{
    SUITE("Location — client_max_body_size");
    Config c("test/config-parser-test/location-client-max-body-size.txt");
    ASSERT_EQ("client_max_body_size", 8192,
        c.getServers()[0].getLocations()[0].getClientMaxBodySize());
    SUITE_END();
}

static void test_multiple_locations(void)
{
    SUITE("Location — multiple blocks");
    Config c("test/config-parser-test/multiple-locations.txt");
    std::vector<Location> locs = c.getServers()[0].getLocations();
    ASSERT_EQ("location count", (size_t)3, locs.size());
    ASSERT_EQ("loc[0] path", std::string("/"),       locs[0].getPath());
    ASSERT_EQ("loc[1] path", std::string("/api"),    locs[1].getPath());
    ASSERT_EQ("loc[2] path", std::string("/static"), locs[2].getPath());
    SUITE_END();
}

// ═════════════════════════════════════════════════════════════════════════════
//  6. Full realistic config
// ═════════════════════════════════════════════════════════════════════════════

static void test_full_server_block(void)
{
    SUITE("Full — realistic server block");
    Config c("test/config-parser-test/full-server-block.txt");
    ServerConfig s = c.getServers()[0];
    ASSERT_EQ("listen",      std::string("8080"),     s.getListen());
    ASSERT_EQ("server_name", std::string("mysite.com"), s.getServerName());
    ASSERT_EQ("root",        std::string("/var/www"),  s.getRoot());
    ASSERT_EQ("index count", (size_t)2,               s.getIndex().size());
    ASSERT_EQ("cmbs",        2048,                    s.getClientMaxBodySize());
    ASSERT_EQ("error pages", (size_t)2,               s.getErrorPage().size());
    ASSERT_EQ("locations",   (size_t)2,               s.getLocations().size());
    SUITE_END();
}

// ═════════════════════════════════════════════════════════════════════════════
//  7. Bad input / errors
// ═════════════════════════════════════════════════════════════════════════════

static void test_empty_file(void)
{
    SUITE("Bad input — empty file");
    ASSERT_EQ("throws on empty file", true, throws_on("test/config-parser-test/empty-file.txt"));
    SUITE_END();
}

static void test_missing_server_keyword(void)
{
    SUITE("Bad input — no server keyword");
    ASSERT_EQ("throws", true, throws_on("test/config-parser-test/missing-server-keyword.txt"));
    SUITE_END();
}

static void test_missing_opening_brace(void)
{
    SUITE("Bad input — missing opening '{'");
    ASSERT_EQ("throws", true, throws_on("test/config-parser-test/missing-opening-brace.txt"));
    SUITE_END();
}

static void test_missing_closing_brace(void)
{
    SUITE("Bad input — missing closing '}'");
    ASSERT_EQ("throws", true, throws_on("test/config-parser-test/missing-closing-brace.txt"));
    SUITE_END();
}

static void test_unknown_server_directive(void)
{
    SUITE("Bad input — unknown server directive");
    ASSERT_EQ("throws", true, throws_on("test/config-parser-test/unknown-server-directive.txt"));
    SUITE_END();
}

static void test_unknown_location_directive(void)
{
    SUITE("Bad input — unknown location directive");
    ASSERT_EQ("throws", true, throws_on("test/config-parser-test/unknown-location-directive.txt"));
    SUITE_END();
}

static void test_missing_semicolon_listen(void)
{
    SUITE("Bad input — missing ';' after listen");
    ASSERT_EQ("throws", true, throws_on("test/config-parser-test/missing-semicolon-listen.txt"));
    SUITE_END();
}

static void test_missing_semicolon_root(void)
{
    SUITE("Bad input — missing ';' after root");
    ASSERT_EQ("throws", true, throws_on("test/config-parser-test/missing-semicolon-root.txt"));
    SUITE_END();
}

static void test_invalid_autoindex_value(void)
{
    SUITE("Bad input — invalid autoindex value");
    ASSERT_EQ("throws", true, throws_on("test/config-parser-test/invalid-autoindex-value.txt"));
    SUITE_END();
}

static void test_invalid_method(void)
{
    SUITE("Bad input — invalid method in location");
    ASSERT_EQ("throws", true, throws_on("test/config-parser-test/invalid-method.txt"));
    SUITE_END();
}

static void test_non_numeric_cmbs(void)
{
    SUITE("Bad input — non-numeric client_max_body_size");
    ASSERT_EQ("throws", true, throws_on("test/config-parser-test/non-numeric-cmbs.txt"));
    SUITE_END();
}

static void test_non_numeric_error_page_code(void)
{
    SUITE("Bad input — non-numeric error_page code");
    ASSERT_EQ("throws", true, throws_on("test/config-parser-test/non-numeric-error-page-code.txt"));
    SUITE_END();
}

static void test_location_missing_opening_brace(void)
{
    SUITE("Bad input — location missing '{'");
    ASSERT_EQ("throws", true, throws_on("test/config-parser-test/location-missing-opening-brace.txt"));
    SUITE_END();
}

static void test_location_missing_closing_brace(void)
{
    SUITE("Bad input — location missing '}'");
    ASSERT_EQ("throws", true, throws_on("test/config-parser-test/location-missing-closing-brace.txt"));
    SUITE_END();
}

// ═════════════════════════════════════════════════════════════════════════════
//  Entry
// ═════════════════════════════════════════════════════════════════════════════

void run_config_parser_tests(void)
{
    std::cout << C_BOLD C_CYAN
              << "\n╔══════════════════════════════════════╗\n"
              << "║       CONFIG PARSER TESTS            ║\n"
              << "╚══════════════════════════════════════╝"
              << C_RESET "\n";

    // Basic directives
    test_empty_server_block();
    test_single_listen();
    test_single_server_name();
    test_single_root();
    test_client_max_body_size();

    // Multiple servers
    test_two_servers();
    test_three_servers();

    // Index
    test_single_index();
    test_multiple_index();

    // Error pages
    test_error_page_404();
    test_multiple_error_pages();

    // Location blocks
    test_empty_location();
    test_location_root();
    test_location_methods_get_only();
    test_location_methods_all();
    test_location_autoindex_on();
    test_location_autoindex_off();
    test_location_index();
    test_location_upload_store();
    test_location_cgi_pass();
    test_location_cgi_extension();
    test_location_return();
    test_location_client_max_body_size();
    test_multiple_locations();

    // Full config
    test_full_server_block();

    // Bad input
    test_empty_file();
    test_missing_server_keyword();
    test_missing_opening_brace();
    test_missing_closing_brace();
    test_unknown_server_directive();
    test_unknown_location_directive();
    test_missing_semicolon_listen();
    test_missing_semicolon_root();
    test_invalid_autoindex_value();
    test_invalid_method();
    test_non_numeric_cmbs();
    test_non_numeric_error_page_code();
    test_location_missing_opening_brace();
    test_location_missing_closing_brace();
}
