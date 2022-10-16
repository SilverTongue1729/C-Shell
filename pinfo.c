#include "pinfo.h"

#include "headers.h"
#include "prompt.h"

extern char* fmlsh_string;

void pinfo(char* root, char* pid_str) {
  if (strcmp(pid_str, "self") && atoi(pid_str) == 0) {
    fprintf(stderr, "%s: numeric argument required\n", fmlsh_string);
    return;
  }

  char path[PATH_MAX];
  sprintf(path, "/proc/%s/stat", pid_str);
  FILE* fp = fopen(path, "r");
  if (fp == NULL) {
    perror("Error");
    return;
  }

  int pid, memory, tpgid, par_grp;
  char status;
  fscanf(fp, "%d", &pid);
  fscanf(fp, "%*s ");
  fscanf(fp, "%c", &status);
  fscanf(fp, "%*s");
  fscanf(fp, "%d", &par_grp);
  fscanf(fp, "%*s %*s");
  fscanf(fp, "%d", &tpgid);
  fscanf(fp, "%*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s");  // 14 strings
  fscanf(fp, "%d", &memory);

  printf("pid : %d\n", pid);
  printf("Process Status : %c%c\n", status, (par_grp == tpgid) ? '+' : ' ');
  printf("Memory : %d\n", memory);

  fclose(fp);

  sprintf(path, "/proc/%s/exe", pid_str);

  char path2[PATH_MAX];
  int len;
  len = readlink(path, path2, PATH_MAX);
  path2[len] = '\0';

  set_path(root, path2, path); // add ~ if child

  printf("Executable Path : %s\n", path);
}