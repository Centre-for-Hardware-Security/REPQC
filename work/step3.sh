echo $1
../bin/relic --net ../bench/$1.bench  --out ../results/$1.zscore --feat ../aux/features.zscore.txt
