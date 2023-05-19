echo $1
../bin/relic_pca --net ../bench/$1.bench --out ../results/$1.grouping --feat ../aux/features.groups.txt --persplt 0.001
