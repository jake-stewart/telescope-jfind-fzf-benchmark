#!/usr/bin/env python3

import sys

def fuzzerize(path):
    parts = path.split("/")
    output = ""
    for part in parts:
        output += part[:2]
    return output

fuzzerized = fuzzerize(sys.argv[1])
assert len(fuzzerized) >= 6
print(fuzzerized)

