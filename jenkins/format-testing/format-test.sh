#!/bin/bash

# Format the unformatted test file.
clang-format-12 -i jenkins/format-testing/unformatted.cc
# Compare the formatted and unformatted.
if cmp --silent ./jenkins/format-testing/unformatted.cc ./jenkins/format-testing/formatted.cc ; then
    echo "PASSED - Baseline clang format test."
    # Reset the unformatted file.
    git checkout HEAD -- jenkins/format-testing/unformatted.cc
    exit 0  # Passed.
else
    echo "FAILED - Baseline clang format test."
    # Reset the unformatted file.
    git checkout HEAD -- jenkins/format-testing/unformatted.cc
    exit 1  # Failed.
fi