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

@test "Run server and client successfully without errors" {
    # Start the server in the background
    ./dsh -s &
    server_pid=$!

    # Give the server some time to start
    sleep 1

    # Run the client and send commands
    run bash -c './dsh -c <<EOF
    echo "Hello, World!"
    exit
    EOF'

    # Stop the server after the test
    kill $server_pid
}

@test "Client handles invalid command" {
    ./dsh -s &
    server_pid=$!
    sleep 1

    run bash -c './dsh -c <<EOF
invalid_command
exit
EOF'

    [[ "$output" =~ "Command execution failed" ]]

    kill $server_pid
}

@test "Stopping server successfully runs" {
    ./dsh -s &
    server_pid=$!
    sleep 1

    run bash -c './dsh -c <<EOF
stop-server
EOF'

    sleep 1
    ! kill -0 $server_pid 2>/dev/null  # Check if server process is gone
}

@test "Client executes simple pipe command" {
    ./dsh -s &
    server_pid=$!
    sleep 1

    run bash -c './dsh -c <<EOF
ls -l | wc -l
exit
EOF'

    [[ "$output" =~ [0-9]+ ]]  # Expect a numeric output from wc -l

    kill $server_pid
}

@test "Client handles redirects" {
    ./dsh -s &
    server_pid=$!
    sleep 1

    run bash -c './dsh -c <<EOF
echo "Hello World" > test.txt
cat test.txt
exit
EOF'

    # Verify the output contains "Hello World"
    [[ "$output" =~ "Hello World" ]]

    # Clean up
    rm -f test.txt
    kill $server_pid
}

@test "Client executes cd command successfully" {
    ./dsh -s &
    server_pid=$!
    sleep 1

    run bash -c './dsh -c <<EOF
cd /
pwd
exit
EOF'

    [[ "$output" =~ "/" ]]

    kill $server_pid
}

@test "Client and Server can be on different ports" {
    ./dsh -s -p 7890 &
    server_pid=$!

    # Give the server some time to start
    sleep 1

    # Run the client and send commands
    run bash -c './dsh -c -p 7890 <<EOF
    echo "Hello, World!"
    exit
    EOF'

    # Stop the server after the test
    kill $server_pid
}