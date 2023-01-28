#!/bin/bash

function create_files() {
  local current_path_tmp=$1
  local files_per_directory=$2
  local file_size=$3

  case $file_size in
    "1B") bytes=1;;
    "1KB") bytes=1024;;
    "1MB") bytes=$(( 1024 * 1024 ));;
    "1GB") bytes=$(( 1024 * 1024 * 1024 ));;
    "RANDOM") bytes=$(( 100000 + ($RANDOM * 250000 / 32767) ));; # random number of bytes between 100 kilobytes and 350 kilobytes
    *) echo "Invalid file size option. Valid options are 1B, 1KB, 1MB, 1GB. File size was set to $file_size."
       return 1;;
  esac

for ((f=0; f<files_per_directory; f++)); do
    local file_name="file_$f.txt"
    printf "%.8d\n" $(echo "obase=2;$f" | bc) | tr -d '\n' > "$current_path_tmp/$file_name"
    if [ $bytes -gt 1 ]; then
        dd if=/dev/zero bs=$((bytes-1)) count=1 status=none >> "$current_path_tmp/$file_name"
    fi
done

}

function setup_directory() {
  local current_path_tmp=$1
  local directories_per_depth=$2
  local files_per_directory=$3
  local recursion_depth=$4
  local current_recursion_depth_tmp=$5
  local file_size=$6

  create_files $current_path_tmp $files_per_directory $file_size

  local dir_counter=0
  while [ $dir_counter -lt $directories_per_depth ]; do
      dir_name="$current_path_tmp/dir_$dir_counter"
      if [ $current_recursion_depth_tmp -lt $recursion_depth ]; then
        mkdir "$dir_name"
        setup_directory $dir_name $directories_per_depth $files_per_directory $recursion_depth $((current_recursion_depth_tmp+1)) $file_size
      fi
      dir_counter=$[$dir_counter +1]
  done
}

current_path=$1
directories_per_depth=$2
files_per_directory=$3
recursion_depth=$4
current_recursion_depth=$5
file_size=$6

setup_directory $current_path $directories_per_depth $files_per_directory $recursion_depth $current_recursion_depth $file_size
