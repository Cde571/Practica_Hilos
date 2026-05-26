CC=gcc
CFLAGS=-O2 -Wall -Wextra -std=c11

SRC_DIR=src
BIN_DIR=bin

all: $(BIN_DIR)/pi_s $(BIN_DIR)/pi_p $(BIN_DIR)/fibonacci $(BIN_DIR)/fibonacci_serial

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/pi_s: $(SRC_DIR)/pi.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $< -lm

$(BIN_DIR)/pi_p: $(SRC_DIR)/pi_p.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $< -pthread -lm

$(BIN_DIR)/fibonacci: $(SRC_DIR)/fibonacci.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $< -pthread

$(BIN_DIR)/fibonacci_serial: $(SRC_DIR)/fibonacci_serial.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf $(BIN_DIR) results/*.csv results/*.png

.PHONY: all clean
