#!/usr/bin/env bash

##########################################
# This script uses valgrind to check for memory errors in `lin`.
#
# This suite considers "success" to be any execution of `lin` without
# a memory error or that isn't terminated by a signal. Hence, even if 
# `lin` returns EXIT_FAILURE, this suite will count it as success as
# long as valgrind doesn't detect any errors.
#
# Some key assumptions:
# 1) `lin` never returns an exit code greater than or equal to 128
# 2) `lin` never returns the value of memerror_code
#
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


# Prints a test summary. Takes THREE arguments: 1) the number of successes (pass)
# 2) the number of crashes, and 3) the total number of tests.
print_summary() {
    local pass="$1"
    local crash="$2"
    local total="$3"

    if [ "$total" -eq 0 ]; then
        echo "No tests were found." >&2
        exit 1
    fi

    echo ""
    printf "${ANSI_BOLD}%sSUMMARY%s${ANSI_RESET}\n" "$line" "$line"
    printf "${ANSI_BOLD}Total tests: %i${ANSI_RESET}\n" "$total"
    printf "${ANSI_BOLD}Successful: %i${ANSI_RESET}\n" "$pass"
    printf "${ANSI_BOLD}Failed: %i${ANSI_RESET}\n" $(( total - pass ))
    printf "${ANSI_BOLD}Crashed: %i${ANSI_RESET}\n" $(( crash ))
    printf "${ANSI_BOLD}${ANSI_CYAN}Overall success %.0f%%${ANSI_RESET}\n" $(( 100*pass/total ))
}


echo ""
printf "${ANSI_BOLD}%sMEMORY TESTS%s${ANSI_RESET}\n" "$line" "$line"

# Expression files must satisfy the pattern below
shopt -s nullglob # By defaul, Bash fills out an empty glob expansion with the pattern string itself. This disables that.
EXPR_FILES=(tests/memory/*.txt)
if [ ${#EXPR_FILES[@]} -eq 0 ]; then
    echo "No expression files were found. Make sure your current working directory is the project root." >&2
    exit 1
fi

# Iterate through the lines in EXPR_FILE and feed each one into `lin`
pass=0
crash=0
total=0

# IFS preserves leading and trailing whitespaces and "$expr" contains the full line.
for file in "${EXPR_FILES[@]}"; do

    echo ""
    printf "${ANSI_BOLD}Running '%s'${ANSI_RESET}\n" "$file"

    while IFS= read -r expr; do

        run_memory_check "$expr"
        status=$?
        (( total++ ))

        # status == memerror_code means a memory error occured. `lin` MUST NOT return memerror_code.
        if [ $status -eq $memerror_code ]; then
            printf "${ANSI_BOLD}./lin %s ${ANSI_RED}FAILED${ANSI_RESET}\n" "$expr"

            valgrind --leak-check=full ./lin "$expr"
        #
        # Unix shells generally reserve exit codes >=128 for terminations by signals.
        # `lin` MUST NOT ever return >=128 on error paths, otherwise a memory-succcessful test
        # might be confused for a signal termination. This should work as long as you run this
        # test script in a mainstream Unix shell.
        #
        elif [ $status -ge 128 ]; then
            printf "${ANSI_BOLD}./lin %s ${ANSI_RED}CRASHED (signal %i)${ANSI_RESET}\n" "$expr" "$(( status - 128 ))"
            (( crash++ ))
        #
        # Any status 128> and not equal to memerror_code is considered either a successful execution
        # or a failure. In either case, no memory error or signal crash occurred so this memory checker
        # counts it assuccessful.
        #
        else
            (( pass++ ))
            printf "${ANSI_BOLD}./lin %s ${ANSI_GREEN}PASS${ANSI_RESET}\n" "$expr"
        fi

    done < "$file"

done

print_summary "$pass" "$crash" "$total" 

