1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

I ensure that all child processes are complete by calling waitpid() allows for the shell to complete all child processes for each forked process in a loop. This would stop zombie processes and helps smoothen processes. The parent shell should always wait until all children have been terminated before moving on to process the next large command.

If waitpid() does not get called then zombie processes would build up, child processes would continue to run, and leaks are prone to happen as the child processes will be orphaned while the parent continues to call a new process.


2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

It is necessary to close unused pipe ends after calling dup2 because keeping the orginal pipe ends open can lead to resource leaks and blocks how the program shouldwork. For example, if a writer process doesn't close its read end of the pipe, and all writer processes terminate, the reader process will never receive an EOF signal because there's still an open write end reference. Similarly, if a reader doesn't close its write end, it may block indefinitely waiting for more data since the pipe remains open for writing. dup2 is used to create a copy of a file descripter, so a proper pipe cleanup by closing unused ends is essential for reliable inter-process communication and resource management.

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

The cd command must be a shell built-in because it needs to modify the current working directory of the shell process itself. If cd were implemented as an external command:

- The child process would change its own working directory, but this change would not affect the parent shell process
- The shell would remain in its original directory after the cd command completes
- Directory changes would not persist between commands
- Environment variables like PWD would not be properly updated in the shell's context
- Relative paths in subsequent commands would resolve incorrectly

This is why directory navigation commands are typically implemented as shell built-ins across all shell implementations.

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

To support arbitrary pipe chains, we could use dynamic arrays or linked lists to store command structures instead of fixed arrays. This way, the program can allocate memory dynamically as needed when parsing pipe sequences. Additionally, dynamic pipe file descriptor allocation is crucial to handle the command count properly. However, there are some trade-offs to consider. Using dynamic allocation may lead to memory fragmentation compared to static allocation, and it introduces more complexity in managing memory and handling errors. There’s also performance overhead due to reallocations, and careful memory cleanup is essential to avoid leaks. The key is finding the right balance between pre-allocating memory and resizing during runtime. A resizable buffer with exponential growth would likely be the most efficient solution, as it reduces the number of reallocations while still supporting flexible command chains.
