#!/bin/sh
IN=./include/version.h
OUT=./genver.h
touch main.c
echo "char *genver=\"" > $OUT
cat $IN | grep -e "^#define" | cut -b 9- | sed "s/\"/\'/g" | \
  sed 's/\\/\\\\/g' >> $OUT
echo "<<Generated at "`date`">>" >> $OUT
echo "by : Áúzoro¹¤×÷ÊÒ\n\";" >> $OUT