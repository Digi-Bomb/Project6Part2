#!/bin/bash
# Client Controller Script
# Clients need IP, port and filename or filepath as input

program=".\Client.exe"
files=("katl-kefd-B737-700.txt" "Telem_2023_3_12 14_56_40.txt" "Telem_2023_3_12 16_26_4" "Telem_czba-cykf-pa28-w2_2023_3_1 12_31_27.txt")

server_ip=192.168.1.1 #placeholder 
server_port=6767

num_clients=5


for ((i=0; i<num_clients; i++)); do
    index=$((i % ${#files[@]})) #rotates through files
    file="${files[$index]}"

    #debug print
    echo "Run $i using $file"

    #runs built command
    "$program" "$file" "$server_ip" "$server_port"

    #can decrease sleep to 0.1 or 0.05
    sleep 1
done
