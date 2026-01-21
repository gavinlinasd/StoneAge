#!/bin/sh
IN=./include/version.h
OUT=./genver.h
touch main.c

# Generate a valid C string using string literal concatenation
printf 'char *genver=\n' > $OUT

grep -e "^#define" "$IN" | cut -b 9- | sed "s/\"/'/g" | sed 's/\\/\\\\/g' | \
  awk '{gsub(/\r/, ""); print "\"" $0 "\\n\""}' >> $OUT

printf '"<<Generated at %s>>\\n"\n' "$(date)" >> $OUT
printf '"by : zoro\\n";\n' >> $OUT
