#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

test_option() {
    local option=$1
    local target=$2
    local expected_pattern=$3
    local test_name=$4

    # Run command and capture output
    output=$(sudo ./ft_ping $option $target 2>&1)
    
    if echo "$output" | grep -q "$expected_pattern"; then
        echo -e "${GREEN}✓ $test_name passed${NC}"
        return 0
    else
        echo -e "${RED}✗ $test_name failed${NC}"
        echo "Expected to find: $expected_pattern"
        echo "Got: $output"
        return 1
    fi
}

# Run option tests
echo "Running option tests..."

test_option "-v" "localhost" "id 0x" "Verbose option test" || exit 1
test_option "-h" "localhost" "Usage:" "Help option test" || exit 1

exit 0