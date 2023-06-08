#!/bin/bash

middle_line() {
    head -n "$(($(wc -l < "$1") / 2))" "$1" | tail -n 1
}

query=$(middle_line "test_input")
input=$(python3 fuzzerize.py "$query")

FZF_DEFAULT_COMMAND="fd" benchmark-vterm \
    --setup="nvim:FZF" \
    --setup-key-delay=50 \
    --post-setup-delay=1000 \
    --input="$input" \
    --input-key-delay=1 \
    --output="> $query" \
    --output-line=15
