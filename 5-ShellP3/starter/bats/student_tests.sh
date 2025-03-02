#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Built-in command: cd" {
    run "./dsh" <<EOF
cd /
pwd
EOF
    
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="/dsh3>dsh3>dsh3>cmdloopreturned0"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Pipeline: ls | grep" {
    run "./dsh" <<EOF
ls | grep dshlib.c
EOF
    
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="dshlib.cdsh3>dsh3>cmdloopreturned0"
    
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"
    
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Multiple pipes" {
    run "./dsh" <<EOF
echo "hello world" | tr "[:lower:]" "[:upper:]" | rev
EOF
    
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="DLROWOLLEHdsh3>dsh3>cmdloopreturned0"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Invalid command" {
    run "./dsh" <<EOF
invalidcommand
EOF
    
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="dsh3>dsh3>dsh3>cmdloopreturned0"
    [ "$status" -eq 0 ]
}

@test "Exit command" {
    run "./dsh" <<EOF
exit
EOF
    
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    
    [ "$status" -eq 0 ]
}