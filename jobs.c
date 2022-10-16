#include "jobs.h"

#include "headers.h"

extern char* fmlsh_string;
extern char* errstr;

void jobs(int* process_pids, char** process_commands, char* flag_str, int nargs) {
  int flag = 0;  // 1 = r, 2 = s;
  if (nargs == 2) {
    if (!strcmp(flag_str, "-r")) {
      flag = 1;
    } else if (!strcmp(flag_str, "-s")) {
      flag = 2;
    } else {
      fprintf(stderr, "%s: %s is not a valid flag\n", fmlsh_string, flag_str);
    }
  }
  FILE* fp;
  char status = 0;
  char path[PATH_MAX];
  for (int i = 0; i < 1000; i++) {
    if (process_pids[i] == 0) continue;
    sprintf(path, "/proc/%d/stat", process_pids[i]);
    fp = fopen(path, "r");
    if (fp == NULL) {
      sprintf(errstr, "%s: Error", fmlsh_string);
      perror(errstr);
    }
    fscanf(fp, "%*d %*s %c", &status);
    if ((flag == 0 || flag == 1) && status != 'T') {
      printf("[%d] Running %s [%d]\n", i, process_commands[i], process_pids[i]);
    }
    if ((flag == 0 || flag == 2) && status == 'T') {
      printf("[%d] Stopped %s [%d]\n", i, process_commands[i], process_pids[i]);
    }
    fclose(fp);
  }
}
