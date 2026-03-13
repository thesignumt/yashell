SRC_DIR := src
INC_DIR := include
BUILD_DIR := build
BIN := app.exe

CC := gcc

CSTD := -std=c17
WARN := -Wall -Wextra -Wpedantic
OPT := -O2

CFLAGS := $(CSTD) $(WARN) $(OPT) -I$(INC_DIR)
DEPFLAGS := -MMD -MP

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

$(BUILD_DIR):
	if (!(Test-Path -Path $(BUILD_DIR))) { New-Item -ItemType Directory -Path $(BUILD_DIR) | Out-Null }

clean:
	if (Test-Path -Path $(BUILD_DIR)) { Remove-Item -Recurse -Force $(BUILD_DIR) }
	if (Test-Path -Path $(BIN)) { Remove-Item -Force $(BIN) }

-include $(DEPS)

.PHONY: all clean
