#!/bin/bash
directory="./host-files"
BINDER_MAJOR_ID=$(cat /proc/devices | grep binder | awk '{print $1}')
while true
do
    for sub_dir in "$directory"/*/; do
	subdir_name=$(basename "$sub_dir")
        echo "c 13:* rwm" >> "$sub_dir/devices.allow"
        echo "c $BINDER_MAJOR_ID:* rwm" >> "$sub_dir/devices.allow"
    done
    sleep 2
done

