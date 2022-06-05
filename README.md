minishell (42 project)
As beautiful as a shell

Description
The objective of this project is to create a simple shell, just like a bash.

Game rules
This shell does not handle unclosed quotes and specified symbols like '' or ';'
It has a working history like real bash
Implemented all kinds of redirections (">", "<" ">>", "<<")
Pipes are working like in real bash
Signals (ctrl-D, ctrl-C, ctrl-) working like in bash
Global virables are implemnted
"$?" (exit status) done
Builtins that are implemented without "execve()" function: \

echo (with -n flag)
cd without flags (with relatives and absolute path)
pwd whithout flags
env without flags
export without flags
unset without flags
exit without flags
Launching
Run make to compile project
Run ./minishell to launch shell
