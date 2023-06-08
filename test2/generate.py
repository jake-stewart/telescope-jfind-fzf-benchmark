#!/usr/bin/env python3

import random

def generate_line():
    chars = []
    for i in range(random.randint(40, 80)):
        chars.append(chr(random.randint(33, 126)))
    return "".join(chars)

for i in range(1_000_000):
    print(generate_line())
