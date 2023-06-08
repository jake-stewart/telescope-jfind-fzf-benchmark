benchmark
=========

Architecture
------------

### Overview
A separate process is created with a pseudoterminal with proper ansi handling.
I launch the fuzzy finder in this process, and start the timer only once neovim is launched and ready benchmarking.
Once every millisecond I check whether the fuzzy finder is completed by checking the terminal output.

### Forkpy and libvterm
A process is using `forkpty`.
We can `read` and `write` to and from `stdin` and `stdout` using the process pseudoterminal file descriptor.
We pass the `stdout` into a `libvterm` `VTerm` instance.
We can then query the screen state with the `VTermScreen` instance.
`libvterm` is what neovim uses for its terminal emulator, along with full standalone terminal emulators such as `pangoterm`. It simply parses escape codes for us, so that we have a valid TUI output to work with.

### Arguments
The program takes a number of arguments:
- `--setup`: A string of keys that should be sent to the pseudoterminal before the benchmark begins.
- `--setup-key-delay`: The delay in milliseconds between keystrokes being sent to the pseudoterminal before the benchmark begins.
- `--post-setup-delay`: The delay after the setup period in millseconds to ensure it is ready for benchmarking. After this delay has finished, the benchmark begins and a timer is started.
- `--input`: A string of keys which are sent to the pseudoterminal during the benchmark.
- `--input-key-delay`: The delay in milliseconds between keystrokes being sent to the pseudoterminal during the benchmark.
- `--output`: A string which must be found on the `VTermScreen` instance for the benchmark to conclude.
- `--output-line`: The line in which the string should be found.

### Program Flow
The program flow is as follows:
1. Send an initial string of keys to get the terminal ready for the benchmark.
   For example, sending "nvim^M:Telescope find_files"
   Notice that a final newline is not sent, since that will cause Telescope to begin processing.
   We do not want it to begin processing yet, we simply want it one key away from being ready.
   Note that the `^M` would be a literal newline. For more information, see the Literal Characters section.
2. Wait a little bit to ensure all processing is finished.
3. Start a timer.
4. Send a string of keys that simulates user input.
   This will be a newline to start the fuzzy finder, and then the query being tested.
5. Once per millisecond, we query the `VTermScreen` to see whether it contains a string which signifies a match has been found.
   For example, telescope will have a "> " followed by the match on the last line of the results box. We will query that line for the match every millisecond until it appears.
6. Stop the timer and print the ellapsed milliseconds.

I repeat these steps 5 times for each test.
The first two results are discarded as a warm up measure.

### Literal Characters
The following literal characters may appear for the `--input` argument.
- `^M`: Newline (Create this in vim by typing `<C-V><CR>` in insert mode.)
- `^H`: Backspace (create this in vim by typing `<C-V><C-H>` in insert mode.)
- `^F`: ACK (Ctrl-F) (create this in vim by typing `<C-V><C-F>` in insert mode.)
If you see them in the Benchmark Vterm Command section of a test,
know that they would be replaced by their literal counterparts.

### Test input
I will write a python script to generate the input for each test.
I will provide the python script alongside each test.
Since python is slow, I will redirect the output of the script into a file, and simply cat the file.
This will remove any bottleneck involved with the input generation process.

To make each program read my test input as easily as possible, I use the following bash script, where `test_input` is the redirected output of the python scripts:

```bash
#!/bin/bash
cat ~/telescope-jfind-fzf-benchmark/test_input
```

This script will be called `fd` and the fuzzy finders will not be aware that it is not the real `fd` binary.
It will behave as they expect and output the test input.
In the case of FZF, I will set `FZF_DEFAULT_COMMAND` to `fd`, so that it also uses my custom `fd` script.

Telescope and jfind both use `fd` to list files. FZF can use `fd` by setting `FZF_DEFAULT_COMMAND`.
To carry out these test, I will be creating a bash script called `fd` which will cat out the test input.

Finally, the query will always be for an item in the middle of the input items.
This is to test the fuzzy finder's ability to match an item before it has even finished reading all the items.

Fuzzy Finders Tested
--------------------
### Tested
- Telescope using `:Telescope find_files`, which reads from the replaced `fd`.
    - I tested using the fzy and fzf extensions, with **no** noticeable performance increase.
      Maybe the bottleneck is with telescope's slow read speed. I will leave
      their results out for simplicity.
- jfind using `^F`, which reads from the replaced `fd`.

### Not Tested
 - I will not test ctrlp since it is vimscript and obviously slow.
 - I will not test fzy since it is not a streaming fuzzy finder. Try to fun fzy in
   your home directory and you will see that it is a feasibly impossible task for
   it.
 - I will not test CommandT since it is not a streaming fuzzy finder. Try to
   fun CommandT in your home directory and you will see that it is a feasibly
   impossible task for it.

Tests
-----

### Test 1: Sequence

My original test, which received valid critisism, was using `seq` to generate the items.
The major concern was that it was not realistic and the order could have skewed the results.
I will include this test anyway.

#### Python script
```python
#!/usr/bin/env python3

for i in range(1_000_000):
    print(f'{i:,}')
```

#### Input Visualisation
1,000,000 lines:
```
...
4,830
4,831
4,832
4,833
4,834
...
```

#### Commands

##### Telescope
```
#!/bin/bash

middle_line() {
    head -n "$(($(wc -l < "$1") / 2))" "$1" | tail -n 1
}
input=$(middle_line "test_input")

benchmark-vterm \
    --setup="nvim^M:Telescope find_files" \
    --setup-key-delay=50 \
    --post-setup-delay=1000 \
    --input="^M$input" \
    --input-key-delay=1 \
    --output="> $input" \
    --output-line=17
```

##### jfind
```
#!/bin/bash

middle_line() {
    head -n "$(($(wc -l < "$1") / 2))" "$1" | tail -n 1
}
input=$(middle_line "test_input")

benchmark-vterm \
    --setup="nvim^M" \
    --setup-key-delay=50 \
    --post-setup-delay=1000 \
    --input="^F$input" \
    --input-key-delay=1 \
    --output="* $input" \
    --output-line=22
```

### FZF
```
#!/bin/bash

middle_line() {
    head -n "$(($(wc -l < "$1") / 2))" "$1" | tail -n 1
}
input=$(middle_line "test_input")

FZF_DEFAULT_COMMAND="fd" benchmark-vterm \
    --setup="nvim^M:FZF" \
    --setup-key-delay=50 \
    --post-setup-delay=1000 \
    --input="^M$input" \
    --input-key-delay=1 \
    --output="> $input" \
    --output-line=15
```

### Results (millseconds)
| Fuzzy Finder | Test 3 | Test 4 | Test 5 | Average |
|--------------|-------:|-------:|-------:|--------:|
| Telescope    | 14,089 | 13,943 | 13,999 | 14,010  |
| fzf-native   | 14,164 | 14,108 | 14,131 | 14,134  |
| jfind        | 98     | 100    | 103    | 100     |
| FZF          | 152    | 150    | 131    | 144     |

jfind was:
 - 14,010% as fast as Telescope
 - 144% as fast as FZF


### Test 2: Random Ascii
To mitigate the structured output of an ordered sequence, we will generate
random ascii.

#### Python script
```python
#!/usr/bin/env python3

import random

def generate_line():
    chars = []
    for i in range(random.randint(40, 80)):
        chars.append(chr(random.randint(32, 126)))
    return "".join(chars)

for i in range(1_000_000):
    print(generate_line())
```

#### Input Visualisation
1,000,000 lines:
```
...
>#ePyV~3w_#o[tJ9ShGrk>5jox@d`j&|h1|/WoKr&6#fM3_2IY\|7'{|ND,K4^kih(L)e@=s+
wE69q=zlO/lW:g[N2"L6#bGWJtj HO:Y;`zsn$Ug(<I,O#DSwq3b$4jFQ9T4+N&x(L.I8%IY-?`q&
'z%y^9=d6\?W%)vv,)4gRu+]7?3X?!UQikAN,k?Op$DV.(/5}jque!LJG4_{d+TN,Klb(40uhc$6G)x
J;S@L5lcf-$Kv|dSNyrDPWy_5e /E.}MCbC ;&y+[[aI"Qv2iTM6hu3P$KyfBu151W`._DjEIJ)?
...
```

#### Commands

##### Telescope
```
#!/bin/bash

middle_line() {
    head -n "$(($(wc -l < "$1") / 2))" "$1" | tail -n 1
}
input=$(middle_line "test_input")

benchmark-vterm \
    --setup="nvim^M:Telescope find_files" \
    --setup-key-delay=50 \
    --post-setup-delay=1000 \
    --input="^M$input" \
    --input-key-delay=1 \
    --output="> $input" \
    --output-line=17
```

##### jfind
```
#!/bin/bash

middle_line() {
    head -n "$(($(wc -l < "$1") / 2))" "$1" | tail -n 1
}
input=$(middle_line "test_input")

benchmark-vterm \
    --setup="nvim^M" \
    --setup-key-delay=50 \
    --post-setup-delay=1000 \
    --input="^F$input" \
    --input-key-delay=1 \
    --output="* $input" \
    --output-line=22
```

### FZF
```
#!/bin/bash

middle_line() {
    head -n "$(($(wc -l < "$1") / 2))" "$1" | tail -n 1
}
input=$(middle_line "test_input")

FZF_DEFAULT_COMMAND="fd" benchmark-vterm \
    --setup="nvim^M:FZF" \
    --setup-key-delay=50 \
    --post-setup-delay=1000 \
    --input="^M$input" \
    --input-key-delay=1 \
    --output="> $input" \
    --output-line=15
```

### Results (milliseconds)
| Fuzzy Finder | Test 3 | Test 4 | Test 5 | Average |
|--------------|-------:|-------:|-------:|--------:|
| Telescope    | 28,722 | 29,236 | 29,011 | 28,990  |
| jfind        | 179    | 177    | 174    | 177     |
| FZF          | 162    | 164    | 167    | 164     |

jfind was:
 - 16,378% as fast as Telescope
 - 92% as fast as FZF

### Test 3: Dummy Paths
This test will mimic file paths by generating random strings of alphanumeric
characters and slashes.

#### Python script
```python
#!/usr/bin/env python3

import random
import os

with open("wordlist.txt", "r") as f:
    words = f.read().split("\n")

def random_line():
    line = []
    for i in range(random.randint(1, 5)):
        line.append(random.choice(words))
    return "/".join(line)

for i in range(1_000_000):
    print(random_line())
```

#### Input Visualisation
1,000,000 lines:
```
...
consider/here/ground/on/road
bird/water
great/was/answer/pull/problem
cry
hole/too/bear/reply
...
```

#### Commands

##### Telescope
```
#!/bin/bash

middle_line() {
    head -n "$(($(wc -l < "$1") / 2))" "$1" | tail -n 1
}
input=$(middle_line "test_input")

benchmark-vterm \
    --setup="nvim^M:Telescope find_files" \
    --setup-key-delay=50 \
    --post-setup-delay=1000 \
    --input="^M$input" \
    --input-key-delay=1 \
    --output="> $input" \
    --output-line=17
```

##### jfind
```
#!/bin/bash

middle_line() {
    head -n "$(($(wc -l < "$1") / 2))" "$1" | tail -n 1
}
input=$(middle_line "test_input")

benchmark-vterm \
    --setup="nvim^M" \
    --setup-key-delay=50 \
    --post-setup-delay=1000 \
    --input="^F$input" \
    --input-key-delay=1 \
    --output="* $input" \
    --output-line=22
```

### FZF
```
#!/bin/bash

middle_line() {
    head -n "$(($(wc -l < "$1") / 2))" "$1" | tail -n 1
}
input=$(middle_line "test_input")

FZF_DEFAULT_COMMAND="fd" benchmark-vterm \
    --setup="nvim^M:FZF" \
    --setup-key-delay=50 \
    --post-setup-delay=1000 \
    --input="^M$input" \
    --input-key-delay=1 \
    --output="> $input" \
    --output-line=15
```

### Results (millseconds)
| Fuzzy Finder | Test 3 | Test 4 | Test 5 | Average |
|--------------|-------:|-------:|-------:|--------:|
| Telescope    | 14,491 | 14,454 | 14,443 | 14,462  |
| jfind        | 110    | 105    | 105    | 107     |
| FZF          | 166    | 155    | 189    | 170     |

jfind was:
 - 13,515% as fast as Telescope
 - 158% as fast as FZF

### Test 4: Fuzzy Dummy Paths
It is not very fuzzy to search for the complete filepath.
This test will generate a fuzzy string for the match by only using the first 2 characters
of the chosen filepath. For example, "/foo/bar/baz" will be searched by "fobaba".

#### Python Script
```python
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
```

#### Fuzzerize.py
```python
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
```

#### Input Visualisation
1,000,000 lines:
```
...
consider/here/ground/on/road
bird/water
great/was/answer/pull/problem
cry
hole/too/bear/reply
...
```

#### Commands

##### Telescope
```
#!/bin/bash

middle_line() {
    head -n "$(($(wc -l < "$1") / 2))" "$1" | tail -n 1
}

query=$(middle_line "test_input")
input=$(python3 fuzzerize.py "$query")

benchmark-vterm \
    --setup="nvim^M:Telescope find_files" \
    --setup-key-delay=50 \
    --post-setup-delay=1000 \
    --input="^M$input" \
    --input-key-delay=1 \
    --output="> $query" \
    --output-line=17
```

##### jfind
```
#!/bin/bash

set -e

middle_line() {
    head -n "$(($(wc -l < "$1") / 2))" "$1" | tail -n 1
}

query=$(middle_line "test_input")
input=$(python3 fuzzerize.py "$query")

benchmark-vterm \
    --setup="nvim^M" \
    --setup-key-delay=50 \
    --post-setup-delay=1000 \
    --input="^F$input" \
    --input-key-delay=1 \
    --output="* $query" \
    --output-line=22
```

### FZF
```
#!/bin/bash

middle_line() {
    head -n "$(($(wc -l < "$1") / 2))" "$1" | tail -n 1
}

query=$(middle_line "test_input")
input=$(python3 fuzzerize.py "$query")

FZF_DEFAULT_COMMAND="fd" benchmark-vterm \
    --setup="nvim^M:FZF" \
    --setup-key-delay=50 \
    --post-setup-delay=1000 \
    --input="^M$input" \
    --input-key-delay=1 \
    --output="> $query" \
    --output-line=15
```

### Results (millseconds)
| Fuzzy Finder | Test 3 | Test 4 | Test 5 | Average |
|--------------|-------:|-------:|-------:|--------:|
| Telescope    | 14,606 | 14,530 | 14,569 | 14,568  |
| jfind        | 109    | 108    | 105    | 107     |
| FZF          | 160    | 171    | 150    | 160     |

jfind was:
 - 13,615% as fast as Telescope
 - 150% as fast as FZF

Conclusion
----------
At its worst, jfind is 13,515% as fast as telescope. My claim that jfind is 100x
faster than telescope is now incorrect. Jfind is over 130x faster than telescope.

I am too lazy to benchmark my claim about 1/2 the memory usage of fzf.
You can look at the fzf source code `item.go` file and see they require 56 bytes per item.
Meanwhile, jfind `item.hpp` only requires 16 bytes per item.
Not only this, but fzf memory usage increases the more you use it.
