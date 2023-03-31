#!/bin/bash

bench() {
    current_path=$1
    directories_per_depth=$2
    files_per_directory=$3
    recursion_depth=$4
    current_recursion_depth=$5
    file_size=$6

    mkdir data
    ./test_data.sh $1 $2 $3 $4 $5 $6

    total_test_data_size=$(du -sh $1 | cut -f1 -d$'\t')
    total_directory_count=$(find ./data -mindepth 1 -type d | wc -l)
    total_file_count=$(find ./data -mindepth 1 -type f | wc -l)
    empty_file_count=$(find ./data -type f -empty -exec echo {} \; | wc -l)

    echo "Mode: $file_size"
    echo "Directories: $total_directory_count"
    echo "Files: $total_file_count"
    echo "Total test data size on disk: $total_test_data_size"
    echo "Empty Files: $empty_file_count"

    hyperfine \
    --export-markdown reports/benchmark_$6.md \
    --warmup 3 \
    --runs 5 \
    'find ./data ! -empty -type f -exec md5sum {} + | sort | uniq -w32 -dD' \
    'ddk_dev -p data' \
    'ddk -p data'

    rm -rf ./data

    sed -i "1i\Files: $total_file_count" reports/benchmark_$6.md
    sed -i "1i\Directories: $total_directory_count" reports/benchmark_$6.md
    sed -i "1i\Total test data size on disk: $total_test_data_size" reports/benchmark_$6.md
    sed -i "1i\Mode: $file_size" reports/benchmark_$6.md
}

mkdir reports

current_path="./data"
directories_per_depth=2000
files_per_directory=2
recursion_depth=1
current_recursion_depth=0
file_size="RANDOM"
bench $current_path $directories_per_depth $files_per_directory $recursion_depth $current_recursion_depth $file_size

current_path="./data"
directories_per_depth=0
files_per_directory=3
recursion_depth=0
current_recursion_depth=0
file_size="1GB"
bench $current_path $directories_per_depth $files_per_directory $recursion_depth $current_recursion_depth $file_size

current_path="./data"
directories_per_depth=10
files_per_directory=1
recursion_depth=3
current_recursion_depth=0
file_size="1MB"
bench $current_path $directories_per_depth $files_per_directory $recursion_depth $current_recursion_depth $file_size

current_path="./data"
directories_per_depth=10
files_per_directory=10
recursion_depth=3
current_recursion_depth=0
file_size="1KB"
bench $current_path $directories_per_depth $files_per_directory $recursion_depth $current_recursion_depth $file_size

echo "# Benchmark Results 📊⏱️📐" >> reports/benchmark.md
cat reports/benchmark_RANDOM.md >> reports/benchmark.md
echo "" >> reports/benchmark.md
cat reports/benchmark_1GB.md >> reports/benchmark.md
echo "" >> reports/benchmark.md
cat reports/benchmark_1MB.md >> reports/benchmark.md
echo "" >> reports/benchmark.md
cat reports/benchmark_1KB.md >> reports/benchmark.md
