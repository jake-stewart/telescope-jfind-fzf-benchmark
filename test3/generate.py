#!/usr/bin/env python3

import random
import os

with open("wordlist.txt", "r") as f:
    words = f.read().split("\n")

def random_line():
    line = []
    for i in range(random.randint(3, 5)):
        line.append(random.choice(words))
    return "/".join(line)

for i in range(1_000_000):
    print(random_line())
