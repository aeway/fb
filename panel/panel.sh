#!/bin/sh


while true; do
    echo $(acpi -b | cut -d ',' -f 2) $(date '+%H:%M')
    sleep 1
done | ./a.out
