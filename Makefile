TARGET := n_puzzle
SRCS_DIR := ./srcs
OBJS_DIR := ./objs
CC := clang++
OS := $(shell uname)
CFLAGS := -Wall -Wextra -Werror -Wno-missing-braces -std=c++11 -O2 # Optimization flag
iw : CFLAGS := -w -std=c++11
LIBS :=
HEADERS := -I./includes/

# ifeq ($(OS), Darwin)
# 	LIBS +=
# 	HEADERS +=
# else
# 	LIBS +=
# 	HEADERS +=
# endif

# Colors
RESET := \033[0m
CYAN := \033[36;01m
GREEN := \033[32;01m
ERROR := \033[31;01m
WARN := \033[33;01m
# Formatting
CLEAR_LINE := \033[2K
MOVE_CURSOR_UP := \033[1A

SRCS := $(shell find $(SRCS_DIR) -name *.cpp)
OBJS := $(patsubst $(SRCS_DIR)%.cpp,$(OBJS_DIR)%.o,$(SRCS))

all: $(OBJS_DIR) $(TARGET)

iw: all

$(OBJS_DIR):
	@mkdir -p ./objs

$(TARGET): $(OBJS)
	@$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	@echo "$(CYAN)Successfully compiled $(TARGET).$(RESET)"

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp
	@$(CC) $(CFLAGS) -c $^ -o $@ $(HEADERS)

clean:
	@rm -rf objs/

fclean: clean
	@rm -f $(TARGET)

re: fclean
	@make all

.PHONY: all clean fclean re iw
