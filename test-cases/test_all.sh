#!/bin/bash

# Clean up directories and files created by previous test runs
cleanup() {
    rm -rf toto test tata testdir 2>/dev/null
    rm -f f1 f2 f 2>/dev/null
}

# Initial cleanup
cleanup

for test in echo ls mkdir oneline oneline2 prompt run badcommand set source; do
    echo "Testing $test..."
    cleanup
    if ../project/src/mysh < ${test}.txt | diff -q - ${test}_result.txt > /dev/null; then
        echo "✓ $test PASSED"
    else
        echo "✗ $test FAILED"
        echo "Differences:"
        ../project/src/mysh < ${test}.txt | diff - ${test}_result.txt
    fi
    cleanup
    echo ""
done

# Final cleanup
cleanup