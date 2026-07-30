# Set C standard according to compiler version
CC = gcc
USE_C23 := $(shell echo "int main(void) { return 0; }" | $(CC) -std=c23 -x c - -o /dev/null 2>/dev/null; echo $$?)

ifeq ($(USE_C23),0)
	CSTD := c23
else
	CSTD := c2x
endif

CFLAGS = -std=$(CSTD) -Wall -Wextra -Werror -pedantic-errors -g
CPPFLAGS = -Isrc

APP_TARGET = lin

SRC_DIR = src
OBJ_DIR = build

APP_SRCS = $(wildcard $(SRC_DIR)/*.c)
APP_OBJS = $(APP_SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

COMPILE.c = $(CC) $(CPPFLAGS) $(CFLAGS)
LINK = $(CC)

all: $(APP_TARGET)

.PHONY: all clean

$(APP_TARGET): $(APP_OBJS)
	$(LINK) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	$(COMPILE.c) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(APP_TARGET)

