NAME := webserv

# ============================================================================ #
#                               COMPILOR & FLAGS                                  #
# ============================================================================ #

MAKE_CMD ?= $(MAKE)
CXX := c++
CXXFLAGS := -Wall -Wextra -Werror
CXXFLAGS += -std=c++98
DEBUG_FLAGS := -DDEBUG -O0

# ============================================================================ #
#                                DIRECTORIES                                   #
# ============================================================================ #

SRC_DIR := src
TEST_DIR := test
OBJ_DIR := objs

# ============================================================================ #
#                                  SOURCES                                     #
# ============================================================================ #

#Main
# MAIN_SRC := $(SRC_DIR)/main.c

# Get all .cpp files recursively
SRCS := $(shell find $(SRC_DIR) -name '*.cpp')

# Get all test .cpp files recursively
TEST_SRCS := $(shell find $(TEST_DIR) -name '*.cpp') $(SRCS)

#Convert source files to object files
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

TEST_SCRIPT = test.sh

# ============================================================================ #
#                                   COLORS                                     #
# ============================================================================ #

#color variables
RED := \033[0;31m
GREEN := \033[0;32m
YELLOW := \033[0;33m
BLUE := \033[0;34m
MAGENTA := \033[0;35m
CYAN := \033[0;36m
WHITE := \033[0;37m
RESET := \033[0m
BOLD := \033[1m

# ============================================================================ #
#                                  🫡RULES                                      #
# ============================================================================ #


#Default target
all: $(NAME)
	@printf "$(GREEN)$(BOLD)✓ Build complete!\n$(RESET)"

#Link the final executable
$(NAME): main.cpp $(OBJS)
	@printf "$(CYAN)Linking $(NAME)...\n$(RESET)"
	@$(CXX) $(CXXFLAGS) main.cpp $(OBJS) -o $(NAME)
	@printf "$(GREEN)$(BOLD)✓ $(NAME) created successfully!🥳🥳🥳\n$(RESET)"

#Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@printf "$(YELLOW)Woah Compiling $<... (ﾉ◕ヮ◕)ﾉ\n$(RESET)"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

#Test suit
test-bin: $(TEST_SRCS)
	@$(CXX) $(CXXFLAGS) $(TEST_SRCS) -o $(TEST_DIR)/run_tests
	$(TEST_DIR)/run_tests

test:
	@if [ ! -f $(TEST_SCRIPT) ]; then \
		printf "$(RED)$(TEST_SCRIPT) not Found\n"; \
		exit 1; \
	fi
	@chmod +x $(TEST_SCRIPT)
	@printf "$(BLUE)Running Tests Files\n$(RESET)"
	@./$(TEST_SCRIPT)

# Debug build (with debug prints)
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: fclean $(NAME)
	@printf "$(YELLOW)⚠ $(NAME) built with debug output enabled$(RESET)"

# ============================================================================ #
#                                 🍻CLEANNING                                    #
# ============================================================================ #

clean:
	@rm -rf $(OBJ_DIR)
	@printf '🧹$(GREEN)Cleaning .o files... m(｡≧ｴ≦｡)m$(RESET)🧹🧹\n'

fclean: clean
	@rm -f $(NAME)
	# rm -f $(NAME_BONUS)
	@printf '🧹🧹$(GREEN)Nothing left...ლ(◉◞౪◟◉ )ლ$(RESET)🧹🧹\n'

re: fclean $(NAME)

# ============================================================================ #
#                              MAKEFILE SETTING                                #
# ============================================================================ #

#not print command
.SILENT:

#Delete target files if command fails
.DELETE_ON_ERROR:

.PHONY: all clean fclean re bonus
