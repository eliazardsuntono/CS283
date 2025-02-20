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

@test "Execute simple command" {
    run "./dsh" <<EOF                
echo Hello, World!
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="Hello,World!dsh2>dsh2>cmdloopreturned0"

    echo "Output: $output"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "List directory contents with ls -l" {
    run "./dsh" <<EOF                
ls -l
EOF

    echo "Output: $output"
    [ "$status" -eq 0 ]
}

@test "Change to home directory and print working directory" {
    run "./dsh" <<EOF
cd ~
pwd
EOF
    
    echo "Output: $output"
    [ "$status" -eq 0 ]
    [[ "$output" =~ "/home/" ]] || [[ "$output" =~ "/Users/" ]]
}

@test "Run a background process" {
    run "./dsh" <<EOF                
sleep 1 &
EOF

    echo "Output: $output"
    [ "$status" -eq 0 ]
}

@test "Execute script file" {
    echo -e '#!/bin/bash\necho "Script executed"' > test_script.sh
    chmod +x test_script.sh

    run "./dsh" <<EOF                
./test_script.sh
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="Scriptexecuteddsh2>dsh2>cmdloopreturned0"

    echo "Output: $output"
    [ "$stripped_output" = "$expected_output" ]
}

@test "Execute multiple commands" {
    run "./dsh" <<EOF
echo "First command"
echo "Second command"
echo "Third command"
EOF

    stripped_output=$(echo "$output" | grep -v "dsh2>" | tr -d '[:space:]')
    expected_output="FirstcommandSecondcommandThirdcommandcmdloopreturned0"

    echo "Output: $output"
    echo "Cleaned output: $stripped_output"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}