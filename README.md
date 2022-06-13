A simple shell program created from working through the projects in the free, online operating systems book by Remzi and Andrea Arpaci-Dusseau (https://pages.cs.wisc.edu/~remzi/OSTEP/).

The shell is rather limited but should perform the basic functionality one would expect. After executing you will be presented with a prompt and the shell will await further input. The shell will happily execute any program it is instructed to provided it has the means to locate the corresponding executable (see the description of the `path` builtin below).  All arguments should be separated by at least one whitespace but it should otherwise be resistant to superfluous whitespace of any kind (be it tabs or spaces). It does not support wildcard expansion (e.g. `rm *.txt` will throw an error) or include most of the bells and whistles of an actual, OS-supporting shell. There are three builtin commands available and their use is documented as follows:

-`exit`: The `exit` command takes no arguments and simply stops the process, returning with a code of 0.

-`cd`: The `cd` command takes one (and only one) argument and changes the current working directory of the calling process to the directory specified by the argument.

-`path`: The `path` command accepts any number of arguments (even 0) and updates the shell's internal list of paths to search for the given command. By default the path is set to `/bin`. As an example, without first updating the path list the command `ls` would attempt to execute the program located at `/bin/ls`. Since that is the only path if the executable did not exist at that location the shell would simply print an error message. By contrast, if one first entered the command `path /bin /usr/bin` and then entered `ls` the shell would walk through the path lists until it found an appropriate executable or print an error if none was found. Be aware that entering `path` with no arguments is a valid input and would leave you unable to execute any commands until the `path` command was given again with appropriate arguments!

## Redirection
The shell supports basic redirection with the `>` character. The `>` character does not require preceding or following whitespace but does require a preceding command to execute (obviously) and expects one (and only one) argument - any more arguments will be treated as an error. For example `ls > output.txt` or `ls>output.txt` are both valid inputs, but `ls > output1.txt output2.txt` would error.

## Parallel Execution
The shell also supports "parallel" execution with the `&` character. As an example `ls > output.txt & wc -l somefile.txt` would `fork()` two new child processes executing their respective programs *before* the parent shell process calls `wait()` to wait for both children to finish.

## Installation
By default the included Makefile builds the program using the GCC toolchain. So long as that is installed simply run `make` in the root directory of this repository and you'll find the executable at bin/wish. Do note: debug symbols are not included in the base executable by default. If you should want to debug this program on your own run the `make debug` rule and attach your debugger to the executable at bin/debug/wish-debug.