1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**: `fgets()` is a good choice for shell input because:
    > - It prevents buffer overflows by allowing us to specify a maximum input length
    > - It handles whitespace and newlines properly, unlike `scanf()`
    > - It reads the entire line of input, which is essential for shell commands
    > - It's more secure than alternatives like `gets()` which has no bounds checking
    > - It returns NULL on EOF, making it easy to handle Ctrl+D for shell termination

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**: Using `malloc()` for `cmd_buff` is necessary because:
    > - The size of user input commands is unpredictable and variable
    > - Fixed-size arrays could waste memory if too large or cause buffer overflows if too small
    > - Dynamic allocation allows the buffer to grow as needed based on actual input
    > - Memory can be properly freed when no longer needed
    > - It's more flexible and safer than using a static buffer size


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  Trimming spaces in command parsing is necessary because:
    > - Commands with leading/trailing spaces would fail to execute as the spaces would be interpreted as part of the command name
    > - System calls like `execvp()` expect exact command names without extra whitespace
    > - It ensures consistent command behavior regardless of user input formatting
    > - It prevents confusion when storing command history or parsing arguments
    > - It matches the behavior of standard shells like bash which automatically trim whitespace

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**: Three key redirection examples to implement:
    > - Output redirection (`>` and `>>`): Redirects STDOUT to a file. Challenge: Must handle file creation/append modes and permissions
    > - Input redirection (`<`): Reads input from a file instead of keyboard. Challenge: Need to properly switch input stream and handle file errors
    > - Error redirection (`2>`): Redirects STDERR to a file. Challenge: Requires separate handling of file descriptors and error streams

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**: Key differences between redirection and piping:
    > - Redirection connects processes with files, while pipes connect processes with other processes
    > - Pipes allow real-time data flow between commands, redirection works with static files
    > - Pipes use memory buffers for IPC, redirection uses filesystem operations
    > - Pipes are bidirectional communication, redirection is typically one-way
    > - Pipes are temporary and memory-based, redirection creates persistent file changes

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**: Separating STDERR from STDOUT is important because:
    > - Allows error messages to be visible even when output is redirected
    > - Enables different handling of errors vs normal output
    > - Prevents error messages from contaminating piped data streams
    > - Makes it easier to log errors separately from regular output
    > - Helps with debugging by clearly distinguishing errors from expected output

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**: For handling command errors:
    > - Display STDERR in a distinct way (through different colors or prefixes)
    > - Implement the `2>&1` operator to merge STDERR into STDOUT when needed
    > - Return appropriate exit codes for error handling
    > - Provide option to log errors separately
    > - Allow customizable error handling through shell configuration