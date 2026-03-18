#!/bin/bash
# Client Controller Script
# Clients need IP, port and filename or filepath as input

program="./client.exe"
files=("File_1" "File_2" "File_3")

server_ip=192.168.1.1 #placeholder 
server_port=67670

num_clients=5


for ((i=0; i<num_clients; i++)); do
    index=$((i % ${#files[@]})) #rotates through files
    file="${files[$index]}"

    #debug print
    echo "Run $i using $file"

    #runs built command
    "$program" "$file" "$server_ip" "$server_port"
done
