SRC_DIR := src
INC_DIR := include
BUILD_DIR := build
BIN := app

CC := gcc
CFLAGS := -Wall -O2 -I$(INC_DIR)

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	@[ -d $(BUILD_DIR) ] || mkdir $(BUILD_DIR)

clean:
	@[ -d $(BUILD_DIR) ] && rm -rf $(BUILD_DIR)
	@[ -f $(BIN) ] && rm -f $(BIN)

.PHONY: all clean
