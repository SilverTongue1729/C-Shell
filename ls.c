#include "ls.h"

#include "headers.h"

extern char* fmlsh_string;
extern char* errstr;
int lflag, aflag;
int tcount, nargs;

void permissions(struct stat* st) {
  mode_t mode = st->st_mode;
  printf("%c%c%c%c%c%c%c%c%c%c",
         S_ISDIR(mode) ? 'd' : '-',
         (mode & S_IRUSR) ? 'r' : '-',
         (mode & S_IWUSR) ? 'w' : '-',
         (mode & S_IXUSR) ? 'x' : '-',
         (mode & S_IRGRP) ? 'r' : '-',
         (mode & S_IWGRP) ? 'w' : '-',
         (mode & S_IXGRP) ? 'x' : '-',
         (mode & S_IROTH) ? 'r' : '-',
         (mode & S_IWOTH) ? 'w' : '-',
         (mode & S_IXOTH) ? 'x' : '-');

  printf(" %3ld", st->st_nlink);

  char name2[1000];

  struct passwd* pwd;
  if ((pwd = getpwuid(getuid())) == NULL) {
    perror("Problem accessing user information for root.\n");
  }
  printf(" %s", pwd->pw_name);

  struct group* grp;
  if ((grp = getgrgid(getgid())) == NULL) {
    perror("Problem accessing user information for root.\n");
  }
  printf(" %s", grp->gr_name);

  printf(" %8ld", st->st_size);

  time_t ts = st->st_mtime;
  struct tm time;
  localtime_r(&ts, &time);
  char time_str[500];
  strftime(time_str, sizeof(time_str), "%c", &time);
  printf(" %s ", time_str);
}

void ls_path(char* path, char* input_name) {
  struct stat st;
  int ret = stat(path, &st);

  if (S_ISDIR(st.st_mode) != 1) {
    if (ret == 0) {  // it is a file
      printf("%s\n", path);
    } else {
      fprintf(stderr, "%s: ls: cannot access '%s': No such file or directory\n", fmlsh_string, input_name);
    }
    return;
  }

  DIR* dir = opendir(path);
  if (!dir) {
    sprintf(errstr, "%s: ls: cannot access '%s'\n", fmlsh_string, input_name);
    perror(errstr);
  }

  if (nargs - tcount > 1) {
    printf("\n%s:\n", input_name);
  }
  struct dirent* entry;
  struct stat st_dir;

  char path2[PATH_MAX];
  int total = 0;
  if (lflag) {
    while ((entry = readdir(dir)) != NULL) {
      strcpy(path2, path);
      strcat(path2, "/");
      strcat(path2, entry->d_name);
      char* name = entry->d_name;

      if (stat(path2, &st_dir)) {
        sprintf(errstr, "%s: Error", fmlsh_string);
        perror(errstr);
        return;
      }
      if (aflag == 0 && name[0] == '.') {
        continue;
      }
      total += st_dir.st_blocks;
    }
    printf("total %d\n", total / 2);

    closedir(dir);
    dir = opendir(path);
  }
  while ((entry = readdir(dir)) != NULL) {
    strcpy(path2, path);
    strcat(path2, "/");
    strcat(path2, entry->d_name);
    char* name = entry->d_name;

    stat(path2, &st_dir);

    if (aflag == 0 && name[0] == '.') {
      continue;
    }
    if (lflag == 1) {
      permissions(&st_dir);
    }
    mode_t mode = st_dir.st_mode;
    if (S_ISDIR(mode)) {
      printf(CLR BOLD BLU "%s\n" CLR RESET, name);
    } else if ((mode & S_IXUSR) || (mode & S_IXGRP) || (mode & S_IXOTH)) {
      printf(CLR BOLD GRN "%s\n" CLR RESET, name);
    } else {
      printf("%s\n", name);
    }
  }
}

void ls(char* root, char** commands, int _nargs) {
  lflag = 0;
  aflag = 0;
  nargs = _nargs;
  tcount = nargs;

  for (int i = 1; i < nargs; i++) {
    if (commands[i][0] != '-') {
      tcount--;
      continue;
    }
    int j = 1;
    while (commands[i][j] != '\0') {
      if (commands[i][j] == 'a')
        aflag = 1;
      else if (commands[i][j] == 'l')
        lflag = 1;
      else {
        fprintf(stderr, "%s: ls: invalid option '%c'\n", fmlsh_string, commands[i][j]);
        return;
      }
      j++;
    }
  }

  for (int i = 1; i < nargs; i++) {
    if (commands[i][0] == '-' && strlen(commands[i]) > 1) {
      continue;
    }
    if (commands[i][0] == '-' && strlen(commands[i]) == 1) {
      fprintf(stderr, "%s: ls: cannot access '-': No such file or directory\n", fmlsh_string);
      continue;
    }

    char path[PATH_MAX] = "\0";
    if (commands[i][0] == '~') {
      strcpy(path, root);
      if (strlen(commands[i]) > 1) strcat(path, commands[i] + 1);
    } else {
      strcat(path, commands[i]);
    }
    ls_path(path, commands[i]);
  }

  if (tcount == nargs) {  // just ls
    char path[PATH_MAX];
    getcwd(path, sizeof(path));
    ls_path(path, "~");
  }
}