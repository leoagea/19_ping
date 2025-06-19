#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

test_invalid_input() {
    local input=$1
    local expected_output=$2
    local test_name=$3

    # Run command and capture output
    output=$(sudo ./ft_ping $input 2>&1)
    
    if echo "$output" | grep -q "$expected_output"; then
        echo -e "${GREEN}✓ $test_name passed${NC}"
        return 0
    else
        echo -e "${RED}✗ $test_name failed${NC}"
        echo "Expected to find: $expected_output"
        echo "Got: $output"
        return 1
    fi
}

# Run invalid input tests
echo "Running invalid input tests..."

test_invalid_input "" "missing host operand" "Empty input test" || exit 1
test_invalid_input "invalid.hostname.test" "unknown host" "Invalid hostname test" || exit 1
test_invalid_input "256.256.256.256" "unknown host" "Invalid IP test" || exit 1

exit 0