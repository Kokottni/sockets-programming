CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = 

SRC_DIR = src
BIN_DIR = bin
SRCS = $(filter-out $(SRC_DIR)/udp_sender.c $(SRC_DIR)/udp_receiver.c, $(wildcard $(SRC_DIR)/*.c))
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%.o,$(SRCS))
SENDER = $(BIN_DIR)/sender
RECEIVER = $(BIN_DIR)/receiver

# default target, build sender and receiver
all: $(BIN_DIR) $(SENDER) $(RECEIVER)

rebuild: clean all

clean:
	rm -rf $(BIN_DIR)

sender: $(BIN_DIR) $(SENDER)
receiver: $(BIN_DIR) $(RECEIVER)

# create bin dir if it doesn't exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# compile sender from .o files
$(SENDER): $(BIN_DIR)/udp_sender.o $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# compile receiver from .o files
$(RECEIVER): $(BIN_DIR)/udp_receiver.o $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# compile source files to object files
$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: all clean rebuild
