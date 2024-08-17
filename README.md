# Operating Systems and Networks Assignment 2,3 - C Shell

Sriteja Reddy Pashya | 2021111019

---

# FML Shell

## Requirements

- GCC compiler
- Linux OS (preferably Ubuntu)

## Running Shell

From the root directory of project, run `make` followed by `./fmlsh`

## File & Functions

- `main.c`
  - Run Shell Loop
  - Executes respective function based on arguments from command

- `command.c`
  - Takes input from user
  - Separates input into commands separated by `;` and `&`, and arguments separated by whitespace

- `prompt.c`
  - Print prompts

- `cd.c`
  - Navigates to a given directory

- `pwd.c`
  - Prints the working directory

- `echo.c`
  - Prints user input after removing excess whitespace

- `discover.c`
  - executes the discover command,
  - Assumes that all file names are enclosed in `""`

- `history.c`
  - Prints history of commands stored in `.fmlsh_history`
  - Doesn't store consecutive commands with the same arguments when extra whitespace is removed
  
- `.fmlsh_history`
  - stores all the commands
  
- `ls.c`
  - Executes the ls command
  
- `pinfo.c`
  - Executes the pinfo command
  
- `runner.c`
  - Executes the processes by forking into a child process

- `headers.h`
  - Contains all necessary headers

## Other Implementations

- `exit` to exit the shell.
- `fml` easter egg.

## Assumptions

- Reasonable assumptions are made for things like length of current path, number of commands, command length, etc. The shell will work perfectly for all "day to day work".

- `echo` will treat everything as normal character.

- File and directory names shouldn't contain spaces or special characters.

- Any error while running a command will result in aborting the command after printing an error message.

- I/O redirection operators `<`, `>`, `>>` and pipes `|` must have whitespace before and after them.
