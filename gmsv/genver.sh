#!/bin/sh
# Generate genver.h: embeds the #define lines of include/version.h as a
# C string literal (one quoted string per line, newlines preserved).
# The original emitted a single multi-line string literal, which no
# compiler since GCC 3.x accepts.
IN=./include/version.h
OUT=./genver.h

{
    echo 'char *genver ='
    grep -e '^#define' "$IN" | cut -b 9- | \
        sed -e "s/\"/'/g" -e 's/\\/\\\\/g' -e 's/^/"/' -e 's/$/\\n"/'
    printf '%s\n' "\"<<Generated at $(date)>>\\n\""
    printf '%s\n' "\"by : 龙zoro工作室\\n\";"
} > "$OUT"
