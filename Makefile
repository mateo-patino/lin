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
COMPILE_DB = compile_commands.json

SRC_DIR = src
OBJ_DIR = build

APP_SRCS := $(shell find $(SRC_DIR) -type f -name '*.c')
APP_OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(APP_SRCS))

COMPILE.c = $(CC) $(CPPFLAGS) $(CFLAGS)
LINK = $(CC)

all: $(APP_TARGET) $(COMPILE_DB)

.PHONY: all clean

$(APP_TARGET): $(APP_OBJS)
	$(LINK) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(COMPILE.c) -MMD -MP -c $< -o $@

$(COMPILE_DB):
	# compile_db.py needs target .json file, curdir, compile command, list of source files
	python3 compile_db.py \
		--filename "$@" \
		--curdir "$(CURDIR)" \
		--compile-cmd "$(COMPILE.c)" \
		--srcs $(APP_SRCS)

clean:
	rm -rf $(OBJ_DIR) $(APP_TARGET)

# Include compile-time generated dependencies
-include $(APP_OBJS:.o=.d)

