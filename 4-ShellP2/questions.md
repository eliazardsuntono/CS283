1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

> **Answer**: We use `fork()` before `execvp()` because `execvp()` replaces the entire process with the new program. If we called `execvp()` directly, our shell would be replaced and terminate after running a single command. By using `fork()`, we create a child process that can run the new program while the parent process (our shell) continues to exist and can accept more commands.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**: If `fork()` fails then that would mean that there was insufficient memory or exceeding process limits. In my code, I implemented this by printing an error using `perror()` and exits the program with 1.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**: `execvp()` finds commands by searching through directories listed in the PATH environment variable. When we run a command like `ls`, `execvp()` checks each directory in PATH sequentially until it finds an executable file matching the command name. The 'p' in `execvp()` specifically indicates that it uses PATH for this search. If the command includes a slash ('/'), PATH is not used and the command is interpreted as a direct path.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn't call it?

    > **Answer**: The `wait()` system call in the parent process serves two crucial purposes: First, it prevents zombie processes by allowing the parent to collect the child's exit status. Second, it ensures proper synchronization by making the parent wait for the child to complete before accepting new commands. Without `wait()`, child processes would become zombies, consuming system resources, and the shell's output might become mixed with subsequent commands, leading to unpredictable behavior and resource leaks. In my code I use `waitpid()` which acts the same way as `wait()` but instead is focused on the functionalities of forking.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  `WEXITSTATUS()` returns the exit status of the child process. By convention the function would return a value of 0, indicating success, whilst other non-zero values represent errors. This is important because the actual exit code needs to be extracted using WEXITSTATUS() because the status value returned by wait() contains other information besides just the exit code (like whether the process was terminated by a signal).

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  The `build_cmd_buff()` function combines parsing strategies from both assignments 1 and 3 to handle quoted and non-quoted arguments effectively. It first skips any leading whitespace before checking for a quote. A size_t variable is used to track the length of the argument, whether quoted or not. As the function parses the input, it increments the length until it finds a closing quote. If a null-terminator is encountered before a closing quote, the function exits with `ERR_CMD_ARGS_BAD`. Finally, `strndup()` is used to allocate space in the argv list based on the tracked length.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  The most significant change was moving away from using `strtok_r()`, as it made handling quoted arguments more difficult. Instead, I implemented a manual parsing approach that iterates through the input character by character, allowing me to properly handle spaces inside quotes while still correctly splitting unquoted arguments. One unexpected challenge was ensuring that quoted arguments were stored correctly without including the quotes themselves while also handling cases where quotes were unclosed. Additionally, managing memory allocation with `strndup()` required careful handling to prevent buffer overflows or memory leaks.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**: Signals are software interrupts that provide a way for handling asynchronous events in Linux. Unlike other IPC methods (pipes, sockets, shared memory) that are used for data transfer, signals are primarily used for process control and notification of specific events. They are lightweight, require no setup, and can be sent between unrelated processes, making them ideal for handling events like termination requests or error conditions.


- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**: Here are three common signals:
    > - SIGTERM (15): The default termination signal. It's a polite way to ask a process to stop, allowing it to clean up before exiting. Used by `kill` command by default.
    > - SIGINT (2): Interrupt signal sent when user presses Ctrl+C. Programs can catch this to perform cleanup before terminating.
    > - SIGKILL (9): Forces immediate process termination. Cannot be caught or ignored. Used as last resort when a process won't respond to SIGTERM.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**: SIGSTOP immediately suspends a process's execution. Unlike SIGINT, SIGSTOP cannot be caught or ignored because it's one of the two signals (along with SIGKILL) that are designed to provide absolute control over processes. This design ensures that system administrators always have a way to control runaway processes. The process remains suspended until it receives a SIGCONT signal.
