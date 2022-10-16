#include "sig.h"
#include "headers.h"

extern char *fmlsh_string;
extern char *errstr;

void sig(int jobn, int sign, int nargs, int *process_pids) {
  if (nargs != 3) {
    fprintf(stderr, "%s: 2 arguments only\n", fmlsh_string);
    return;
  }
  if (process_pids[jobn] == 0) {
    fprintf(stderr, "%s: Job %d doesn't exist\n", fmlsh_string, jobn);
    return;
  }
  printf("hi\n");
  if (kill(process_pids[jobn], sign) != 0) {
    sprintf(errstr, "%s: ", fmlsh_string);
    perror(errstr);
    return;
  }
  printf("hi2\n");
  process_pids[jobn] = 0;
}