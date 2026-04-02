#include "testUtils.hpp"
#include <iostream>

int g_pass = 0;
int g_fail = 0;

void	run_config_parser_tests(void);

int main(void)
{
    // … run_routing_tests(), run_response_tests(), etc.

	run_config_parser_tests();
    int total = g_pass + g_fail;
    std::cout << "\n" C_BOLD;
    if (g_fail == 0)
        std::cout << C_GREEN "  ✓ All " << total << " tests passed." C_RESET "\n\n";
    else
        std::cout << C_GREEN "  " << g_pass << " passed" C_RESET
                  << "  " C_RED C_BOLD << g_fail << " failed" C_RESET
                  << C_GREY "  (total: " << total << ")" C_RESET "\n\n";

    return (g_fail > 0 ? 1 : 0);
}
