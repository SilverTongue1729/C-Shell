#include "command.h"
#include "headers.h"

int cd(char* root, char* cwd, char* newdir, char* prevwd) {
  char path[1000];
  if (newdir[0] == '~') {
    strcpy(path, root);
    if (strlen(newdir) > 2) {
      strcat(path, newdir + 1);
    } else if (strlen(newdir) == 2 && newdir[1] != '/') {
      return -1;
    }
    if (chdir(path) < 0) {
      return -1;
    }
    return 0;
  }
  if (newdir[0] == '-' && strlen(newdir) == 1) {
    if (chdir(prevwd) < 0) {
      return -1;
    }
    printf("%s\n",prevwd);
    return 0;
  }
  if (chdir(newdir) < 0) {
    return -1;
  }
  return 0;
}
