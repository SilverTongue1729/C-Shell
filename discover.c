#include "discover.h"

#include "headers.h"
#include "prompt.h"

extern char* fmlsh_string;

int dflag, fflag, fileflag;
char file_name[PATH_MAX];
char dir_path[PATH_MAX];
char temp[PATH_MAX];
char cwd[PATH_MAX];
char _root[PATH_MAX];

void discover_search(char* path) {
  DIR* dir;
  if (!(dir = opendir(path))) return;

  struct dirent* entry;

  char path2[PATH_MAX];
  char temp_path[PATH_MAX];
  while ((entry = readdir(dir)) != NULL) {
    if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
      continue;
    }
    sprintf(temp_path, "%s/%s", path, entry->d_name);
    set_path(cwd, temp_path, path2);
    if (path2[0] == '~') {
      path2[0] = '.';
    }

    struct stat st2;
    stat(path2, &st2);

    if (S_ISDIR(st2.st_mode)) {
      if (fileflag == 0 && dflag)
        printf("%s\n", path2);
      discover_search(path2);
      continue;
    }

    if (fflag == 0) continue;
    if (fileflag == 0) {
      printf("%s\n", path2);
      continue;
    }

    int len1 = strlen(path2);
    int len2 = strlen(file_name);

    int tempflag = 0;
    for (int i = len1 - 1, j = len2 - 1; i && j; i--, j--) {
      if (path2[i] != file_name[j]) {
        tempflag = 1;
        break;
      }
    }
    if (tempflag) continue;

    printf("%s\n", path2);
  }
}

void discover(char* root, char** commands, int nargs) {
  strcpy(_root, root);
  dflag = 0, fflag = 0, fileflag = 0;

  for (int j = 1; j < nargs; j++) {
    if (!strcmp(commands[j], "-d")) {
      dflag = 1;
      continue;
    }
    if (!strcmp(commands[j], "-f")) {
      fflag = 1;
      continue;
    }
    if (commands[j][0] == '-') {
      fprintf(stderr, "%s: discover: invalid option --  \'%s\'\n", fmlsh_string, commands[j] + 1);
      return;
    }
  }

  if (dflag == 0 && fflag == 0){
    dflag = fflag = 1;
  }

  file_name[0] = '\0';
  dir_path[0] = '\0';

  getcwd(cwd, sizeof(cwd));
  for (int i = 1; i < nargs; i++) {
    int len = strlen(commands[i]);
    if (commands[i][0] == '-') {
      continue;
    }
    if (commands[i][0] == '\"' && commands[i][len - 1] == '\"') {
      fileflag = 1;
      strncpy(file_name, commands[i] + 1, len - 2);
      continue;
    } else if (commands[i][0] == '\"' || commands[i][len - 1] == '\"') {
      fprintf(stderr,
              "%s: discover: Warning, file_name not "
              "enclosed in double quotes\n",
              fmlsh_string);
      continue;
    }

    strcpy(dir_path, commands[1]);
    strcpy(temp, dir_path);

    if (dir_path[0] == '~') {
      sprintf(dir_path, "%s%s", root, temp + 1);
    }
    if (dir_path[0] == '.') {
      sprintf(dir_path, "%s%s", cwd, temp + 1);
    }

    chdir(dir_path);
    if (chdir(dir_path) < 0) {
      strcpy(file_name, dir_path);
      fprintf(stderr, "%s: discover: Target directory -- \'%s\' does not exist\n", fmlsh_string, temp);
      chdir(cwd);
      continue;
    }
  }

  char cwd2[PATH_MAX];
  getcwd(cwd2, sizeof(cwd2));

  if (dflag == 1 && fileflag == 0) {
    printf(".\n");
  }

  discover_search(cwd2);

  chdir(cwd);
}
