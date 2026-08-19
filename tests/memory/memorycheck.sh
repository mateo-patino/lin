#!/usr/bin/env bash

##########################################
# This script uses valgrind to check for memory errors in `lin`.
##########################################


# Check valgrind exists
if ! command -v valgrind >/dev/null 2>&1; then
    cat << END >&2
Error: valgrind was not found. 
Install it or use a Linux-based machine where valgrind is supported.
END
    exit 1
fi


# Check `lin` executable exists in current dir
if [ ! - x "./lin" ]; then
    echo "No 'lin' executable was found in the current working directory. Build 'lin' before running the memory checker." >&2
    exit 1
fi


# memerror_code contains the valgrind's exit status should a memory error be found
# memerror_code cannot be an exit status returned by `lin` on failing execution paths
memerror_code = 76
run_memory_check() {
    local expr="$1"

    valgrind \
        --quiet \
        --leak-check=full \
        --error-exitcode="$memerror_code" \
        ./lin "$expr" >/dev/null 2>&1 
}

line="------------"
ANSI_RESET="\033[0m"
ANSI_BOLD="\033[1m"
ANSI_RED="\033[31m"
ANSI_GREEN="\033[32m"
ANSI_CYAN="\033[36m"

echo ""
printf "${ANSI_BOLD}%sMEMORY TESTS%s${ANSI_RESET}\n" "$line" "$line"

# This file contains expression to feed into `lin`
EXPR_FILE="tests/memory/expressions.txt"
if [ ! -f "$EXPR_FILE" ]; then
    echo "File '$EXPR_FILE' was not found. Make sure your current working directory is the project root."
    exit 1
fi


while IFS= read -r line; do
    echo "$line"
done < "$EXPR_FILE"


