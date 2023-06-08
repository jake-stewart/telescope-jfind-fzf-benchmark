#!/bin/bash

middle_line() {
    head -n "$(($(wc -l < "$1") / 2))" "$1" | tail -n 1
}
input=$(middle_line "test_input")

benchmark-vterm \
    --setup="nvim:Telescope find_files" \
    --setup-key-delay=50 \
    --post-setup-delay=1000 \
    --input="$input" \
    --input-key-delay=1 \
    --output="> $input" \
    --output-line=17
