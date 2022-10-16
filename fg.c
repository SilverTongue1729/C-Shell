#include "fg.h"

#include "headers.h"

extern char *fmlsh_string;
extern char *errstr;

void fg(int pid, int nargs, int *process_pids, char **process_commands) {
  if (nargs != 2) {
    fprintf(stderr, "%s: 2 arguments only\n", fmlsh_string);
    return;
  }
  if (pid < 1 || pid > 999 || process_pids[pid] <= 1) {
    fprintf(stderr, "%s: Invalid Job\n", fmlsh_string);
  }
  if (kill(pid, SIGCONT) < 0) {
    perror("Error");
    return;
  }

  int status;
  setpgid(pid, getpgid(0));

  signal(SIGTTIN, SIG_IGN);
  signal(SIGTTOU, SIG_IGN);

  tcsetpgrp(0, pid);

  if (kill(pid, SIGCONT) < 0) {
    sprintf(errstr, "%s: Error", fmlsh_string);
    perror(errstr);
  }

  waitpid(pid, &status, WUNTRACED);

  tcsetpgrp(0, getpgid(0));

  signal(SIGTTIN, SIG_DFL);
  signal(SIGTTOU, SIG_DFL);

  if (WIFSTOPPED(status))
    return;

  // if (WIFEXITED(status)) {
  for (int i = 1; i < 100; i++) {
    if (process_pids[i] == pid) {
      process_pids[i] = 0;
      free(process_commands[i]);
      break;
    }
  }
  // }
}