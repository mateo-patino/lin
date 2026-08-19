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
if [ ! -x "./lin" ]; then
    echo "No 'lin' executable was found in the current working directory. Build 'lin' before running the memory checker." >&2
    exit 1
fi


# memerror_code contains the valgrind's exit status should a memory error be found
# memerror_code cannot be an exit status returned by `lin` on failing execution paths
memerror_code=76

# Runs valgrind on `expr`. Takes ONE argument only, the expression to feed into lin.
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


# Prints a test summary. Takes TWO arguments: 1) the number of successes (pass)
# 2) the total number of tests.
print_summary() {
    local pass="$1"
    local total="$2"

    if [ "$total" -eq 0 ]; then
        echo "No tests were found." >&2
        exit 1
    fi

    echo ""
    printf "${ANSI_BOLD}%sSUMMARY%s${ANSI_RESET}\n" "$line" "$line"
    printf "${ANSI_BOLD}Total tests: %i${ANSI_RESET}\n" "$total"
    printf "${ANSI_BOLD}Successful: %i${ANSI_RESET}\n" "$pass"
    printf "${ANSI_BOLD}Failed: %i${ANSI_RESET}\n" $(( total - pass ))
    printf "${ANSI_BOLD}${ANSI_CYAN}Overall success %.0f%%${ANSI_RESET}\n" $(( 100*pass/total ))
}


echo ""
printf "${ANSI_BOLD}%sMEMORY TESTS%s${ANSI_RESET}\n" "$line" "$line"

# This file contains expression to feed into `lin`
EXPR_FILE="tests/memory/valid.txt"
if [ ! -f "$EXPR_FILE" ]; then
    echo "File '$EXPR_FILE' was not found. Make sure your current working directory is the project root."
    exit 1
fi

# Iterate through the lines in EXPR_FILE and feed each one into `lin`
pass=0
total=0

# IFS preserves leading and trailing whitespaces and "$expr" contains the full line.
while IFS= read -r expr; do
    run_memory_check "$expr"
    status=$?
    (( total++ ))

    if [[ $status -eq $memerror_code ]]; then
        printf "${ANSI_BOLD}./lin %s ${ANSI_RED}FAILED${ANSI_RESET}\n" "$expr"

        # Rerun without the quiet flag to display valgrind report
        valgrind --leak-check=full ./lin "$expr"
    else
        (( pass++ ))
        printf "${ANSI_BOLD}./lin %s ${ANSI_GREEN}PASS${ANSI_RESET}\n" "$expr"
    fi
done < "$EXPR_FILE"

print_summary "$pass" "$total" 

