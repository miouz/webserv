#ifndef TESTUTILS_HPP
# define TESTUTILS_HPP

# include <iostream>
# include <string>

extern int g_pass;
extern int g_fail;

// ── Colors ────────────────────────────────────────────────────────────────────
# define C_RESET   "\033[0m"
# define C_BOLD    "\033[1m"
# define C_RED     "\033[0;31m"
# define C_GREEN   "\033[0;32m"
# define C_YELLOW  "\033[0;33m"
# define C_BLUE    "\033[0;34m"
# define C_CYAN    "\033[0;36m"
# define C_GREY    "\033[0;90m"

// ── Helpers ───────────────────────────────────────────────────────────────────
# define SUITE(name) \
    std::cout << "\n" C_BOLD C_BLUE "┌─ " name C_RESET "\n";

# define SUITE_END() \
    std::cout << C_GREY "└────────────────────────────" C_RESET "\n";

# define ASSERT_EQ(desc, expected, actual) \
    do { \
        if ((expected) == (actual)) { \
            std::cout << C_GREEN "  ✓ " C_RESET << (desc) << "\n"; \
            g_pass++; \
        } else { \
            std::cout << C_RED "  ✗ " C_BOLD << (desc) << C_RESET "\n" \
                      << C_GREY "    expected : " C_RESET << (expected) << "\n" \
                      << C_GREY "    got      : " C_RESET << (actual)   << "\n"; \
            g_fail++; \
        } \
    } while (0)

// For tests waiting exception
# define ASSERT_THROW(desc, expr) \
    do { \
        bool _threw = false; \
        try { expr; } catch (...) { _threw = true; } \
        if (_threw) { \
            std::cout << C_GREEN "  ✓ " C_RESET << (desc) << "\n"; \
            g_pass++; \
        } else { \
            std::cout << C_RED "  ✗ " C_BOLD << (desc) \
                      << C_RESET C_GREY " (expected exception, none thrown)" C_RESET "\n"; \
            g_fail++; \
        } \
    } while (0)

// For tests waiting rejection from the parser
# define ASSERT_INVALID(desc, parser) \
    do { \
        if (!(parser).isComplete()) { \
            std::cout << C_GREEN "  ✓ " C_RESET << (desc) << "\n"; \
            g_pass++; \
        } else { \
            std::cout << C_RED "  ✗ " C_BOLD << (desc) \
                      << C_RESET C_GREY " (parser accepted an invalid request)" C_RESET "\n"; \
            g_fail++; \
        } \
    } while (0)

#endif
