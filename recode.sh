#!/bin/sh
#echo "Recoding /saac folder.."
#for file in saac/src/*.[ch]; do
#    echo "$file"
#    recode gb18030..utf8 $file
#done
#echo "Done!"

echo "Recoding /gmsv folder.."
for file in `find gmsv -type f`; do
    echo "Processing $file"
    recode gb18030..utf8 $file
done
