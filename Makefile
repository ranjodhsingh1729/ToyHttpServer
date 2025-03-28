vpath %.c src/

CC := gcc
CFLAGS := -Wall -Wextra -pedantic -std=gnu99
SOURCES := server.c logger.c handler.c parser.c status.c router.c strutils.c

BUILD_DIR := build/
INCLUDE_DIR := include/
TARGET_EXEC := server

CPPFLAGS := -I$(INCLUDE_DIR)

$(BUILD_DIR)$(TARGET_EXEC): $(SOURCES)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f $(BUILD_DIR)$(TARGET_EXEC)