#include "bg.h"

#include "headers.h"

extern char *fmlsh_string;
extern char *errstr;

void bg(int pid, int nargs, int *process_pids) {
  if (nargs != 2) {
    fprintf(stderr, "%s: 2 arguments only\n", fmlsh_string);
    return;
  }
  if (pid < 1 || pid > 999 || process_pids[pid] <= 1) {
    fprintf(stderr, "%s: Invalid Job\n", fmlsh_string);
    return;
  }
  if (kill(pid, SIGCONT) < 0) {
    sprintf(errstr,"%s: Error", fmlsh_string);
    perror(errstr);
  }
}