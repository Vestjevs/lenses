CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -Werror -O2
INCLUDES := -Iinclude
SRC_DIR := src
EX_DIR := examples
BIN_DIR := bin

LIB_OBJS := $(SRC_DIR)/byte_stack.o

.PHONY: all clean run

all: $(BIN_DIR)/demo

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c include/byte_stack.h | $(BIN_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BIN_DIR)/demo: $(LIB_OBJS) $(EX_DIR)/main.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) $(LIB_OBJS) $(EX_DIR)/main.c -o $@

run: $(BIN_DIR)/demo
	./$(BIN_DIR)/demo

clean:
	rm -rf $(BIN_DIR) $(SRC_DIR)/*.o