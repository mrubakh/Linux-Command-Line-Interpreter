# Linux-Command-Line-Interpreter

For this project, I implemented a **command line interpreter** or *shell*.  The shell operates by taking in a command (in response to the shell prompt) and then creates a child process that executes the entered command, and then prompts for more user input when finished.
The shell implementation is similar to common Linux interpreter(e.g., bash), but with various simplifications. The shell allows multiple processes to be executed simultaneously and run seemlessly in the background. In terms of functionality, the shell can execute program options (e.g., /bin/ls -l) or "built-in" commands such as 

1. **exit** : Quits the shell immediately

2. **status** : When a user types this as a command, the shell should print a list of processes that are currently running in the background

For example,
````
prompt> status
Processes currently active: 
    [30]: /bin/sleep
    [29]: /bin/sleep
    [28]: /bin/sleep
````
3. **kill**: which checks whether a given child process is running and issues a system call to kill the child process whose PID corresponds with the given PID




