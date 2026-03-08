SRC_DIR := src
INC_DIR := include
BUILD_DIR := build
BIN := app

CC := gcc

CSTD := -std=c17
WARN := -Wall -Wextra -Wpedantic
OPT := -O2

CFLAGS := $(CSTD) $(WARN) $(OPT) -I$(INC_DIR)

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	@[ -d $(BUILD_DIR) ] || mkdir $(BUILD_DIR)

clean:
	@[ -d $(BUILD_DIR) ] && rm -rf $(BUILD_DIR)
	@[ -f $(BIN) ] && rm -f $(BIN)

-include $(DEPS)

.PHONY: all clean
