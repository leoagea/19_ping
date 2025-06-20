###############################################################################
# Colors
###############################################################################
GREEN  := \033[0;32m
YELLOW := \033[1;33m
BLUE   := \033[0;34m
NC     := \033[0m

###############################################################################
# Compiler & Flags
###############################################################################
CC       = gcc
CFLAGS   = -Wall -Wextra -Werror -O2 -MMD -MP -g -fsanitize=address
LDFLAGS  = -lm 
# -MMD and -MP tell the compiler to generate .d (dependency) files for each .c

DEBUG_FLAGS   := -g -fsanitize=address
BUILD_MODE_FILE = $(OBJ_DIR)/.build_mode

###############################################################################
# Project Settings
###############################################################################
TARGET   = ft_ping
SRC_DIR  = srcs
INC_DIR  = inc
OBJ_DIR  = build

###############################################################################
# Sources / Objects
###############################################################################
SRC_FILES := $(shell find $(SRC_DIR) -type f -name '*.c')
			
SRCS      := $(SRC_FILES)

OBJS      := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

DEPS      = $(OBJS:.o=.d)

###############################################################################
# Default Rule
###############################################################################
all: $(TARGET)

###############################################################################
# Linking
###############################################################################
$(TARGET): $(OBJS)
	@echo "$(BLUE)Linking $(TARGET)...$(NC)"
	@$(CC) $(CFLAGS) -I$(INC_DIR) -o $@ $(OBJS) $(LDFLAGS)
	@echo "$(GREEN)Build complete!$(NC)"

###############################################################################
# Object File Compilation
###############################################################################
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "$(YELLOW)Compiling $<...$(NC)"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

###############################################################################
# Cleanup
###############################################################################
clean:
	rm -f $(OBJS) $(DEPS) $(BUILD_MODE_FILE)

fclean: clean
	rm -f $(TARGET)
	rm -rf $(OBJ_DIR)

re: fclean all

###############################################################################
# Others Rules
###############################################################################
debug: CFLAGS := $(DEBUG_FLAGS)
debug:
	@if [ ! -f $(BUILD_MODE_FILE) ] || [ "$$(cat $(BUILD_MODE_FILE))" != "debug" ]; then \
		$(MAKE) fclean; \
		mkdir -p $(OBJ_DIR); \
		echo "debug" > $(BUILD_MODE_FILE); \
	fi
	@$(MAKE) all

help:
	@echo "Available targets:"
	@echo "  all      : Build $(TARGET)"
	@echo "  debug    : Build with debug flags"
	@echo "  release  : Build with optimization"
	@echo "  debug    : Build with debug flags"
	@echo "  release  : Build with optimization"
	@echo "  test     : Run tests with arguments"
	@echo "  tests    : Run all test scripts"
	@echo "  clean    : Remove object files"
	@echo "  fclean   : Remove all generated files"
	@echo "  re       : Rebuild everything"

###############################################################################
# Tests Rules
###############################################################################

TEST_DIR = tests
TEST_SCRIPTS =  $(TEST_DIR)/test_invalid.sh \
				$(TEST_DIR)/test_options.sh

test: $(TARGET)
	@chmod +x tests/*.sh
	@echo "$(BLUE)Running tests...$(NC)"
	@for script in $(TEST_SCRIPTS); do \
		bash $$script || exit 1; \
	done
	@echo "$(GREEN)All tests passed!$(NC)"

###############################################################################
# Dependency Handling
###############################################################################
-include $(DEPS)

###############################################################################
# Phony Targets
###############################################################################
.PHONY: all clean fclean re debug help
