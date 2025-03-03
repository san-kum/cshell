BUILD_DIR = build
CC = gcc
CFLAGS = -Wall -Wextra -g -Isrc/include

all: $(BUILD_DIR)/cshell

$(BUILD_DIR)/cshell: $(BUILD_DIR)/cshell.o $(BUILD_DIR)/builtins.o $(BUILD_DIR)/utils.o
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/cshell.o: src/myshell.c src/include/utils.h src/include/builtins.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/builtins.o: src/builtins.c src/include/builtins.h src/include/utils.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/utils.o: src/utils.c src/include/utils.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

# Create the build directory if it doesn't exist
$(shell mkdir -p $(BUILD_DIR))

.PHONY: all clean
