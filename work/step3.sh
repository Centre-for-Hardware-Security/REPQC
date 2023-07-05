#!/bin/bash

echo $1
if [ $1 = "gmu" ]; then
	echo "WARNING: The gmu circuit causes relic to use a very large amount of memory. You may have to use unlimit or similar linux command"
fi
../bin/relic --net ../bench/$1.bench  --out ../results/$1.zscore --feat ../aux/features.zscore.txt
