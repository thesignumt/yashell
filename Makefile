# Compiler and flags
CC := gcc
CFLAGS := -Wall -Wextra -O2

# Directories
SRC_DIR := src
BUILD_DIR := build
BIN := app

# Sources and objects
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# Default target
all: $(BIN)

# Link objects
$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create build directory if missing
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean
clean:
	rm -rf $(BUILD_DIR) $(BIN)

.PHONY: all clean
