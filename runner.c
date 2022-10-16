#include "runner.h"

#include "headers.h"

extern char* errstr;
extern char* fmlsh_string;
extern int process_time;


int run_command(int background, char** args) {
  int pid = fork();

  if (pid < 0) {  // error
    sprintf(errstr, "%s: Error", fmlsh_string);
    perror(errstr);
    return -1;
  } else if (pid) {  // parent
    if (background) {
      return pid;
    } else {
      int status;
      int wpid = waitpid(pid, &status, WUNTRACED);
      if (WIFSTOPPED(status)) return pid;
      if (WEXITSTATUS(status)) return -1;
    }
  } else {  // child

    char errstr2[1000];
    if (execvp(args[0], args) < 0) {
      sprintf(errstr2, "%s: %s", fmlsh_string, args[0]);
      perror(errstr2);
      exit(1);
    }
  }
}
