#include "prompt.h"

#include "headers.h"

extern int process_time;

void set_path(char* root, char* cwd, char* path) {
  for (int i = 0;; i++) {
    if (root[i] == '\0') {
      path[0] = '~';
      path[1] = '\0';
      break;
    } else if (cwd[i] == '\0' || cwd[i] != root[i]) {
      break;
    }
  }
  if (path[0] == '~')
    strcat(path, cwd + strlen(root));
  else
    strcpy(path, cwd);
}

void prompt(char* root, char* cwd) {
  struct passwd* pwd;
  if ((pwd = getpwuid(getuid())) == NULL) {
    perror("Problem accessing user information for root.\n");
  }
  char hostname[HOST_NAME_MAX + 1];
  if (gethostname(hostname, sizeof(hostname)) == -1) {
    perror("gethostname\n");
  }
  char path[PATH_MAX] = " ";
  set_path(root, cwd, path);
  char timestr[20] = "";
  if (process_time >= 1) {
    sprintf(timestr, CLR BOLD YEL " took %ds" CLR RESET, process_time);
  }
  printf("<" CLR BOLD GRN "%s@%s" CLR RESET ":" CLR BOLD BLU "%s" CLR RESET "%s>", pwd->pw_name, hostname, path, timestr);
  process_time = 0;

  fflush(stdout);
}
