#include "history.h"

#include "headers.h"

extern char* fmlsh_string;
extern char* errstr;

int flag = 0;

void remove_whitespace(char* str) {
  int x = 0;
  for (int i = 0; str[i]; i++) {
    if (!isspace(str[i]) || (i > 0 && !isspace(str[i - 1])))
      str[x++] = str[i];
  }
  if (str[x - 1] != '\n') {
    str[x - 1] = '\n';
  }
  str[x] = '\0';
}

int get_history(char* root, char** commands) {
  char path[PATH_MAX];
  strcpy(path, root);
  strcat(path, "/.fmlsh_history");
  FILE* file = fopen(path, "r");
  if (file == NULL) {
    fprintf(stderr, "%s: Failed to open ~/.fmlsh_history\n", fmlsh_string);
    fprintf(stderr, "%s: Creating ~/.fmlsh_history\n", fmlsh_string);
    file = fopen(path, "w+");
  }

  int len = 0;
  unsigned long size = 1000;
  for (int i = 0; i < 20; i++) {
    commands[i + 1] = NULL;
    if (getline(&commands[i], &size, file) != -1) {
      len++;
    } else {
      break;
    }
  }
  fclose(file);
  return len;
}

void add_history(char* root, char* str) {
  if (!strcmp(str, "\n")) return;

  remove_whitespace(str);

  char* commands[21];
  int len = get_history(root, commands);
  if (len != 0 && !strcmp(str, commands[len - 1])) {
    return;
  }

  char path[PATH_MAX];
  strcpy(path, root);
  strcat(path, "/.fmlsh_history");

  FILE* fp = fopen(path, "w");
  if (fp == NULL) {
    fprintf(stderr, "%s: Failed to open file\n", fmlsh_string);
    return;
  }

  if (len < 20 && len) {
    for (int i = 0; i < len; i++) {
      fprintf(fp, "%s", commands[i]);
    }
  } else {
    for (int i = 0; i < len - 1; i++) {
      fprintf(fp, "%s", commands[i + 1]);
    }
  }
  fprintf(fp, "%s", str);

  fclose(fp);
}

void history(char* root, char* nstr) {
  int n = 10;
  if (nstr) n = atoi(nstr);
  if (n == 0 && strcmp(nstr, "0")) {
    fprintf(stderr, "%s: history: %s: numeric argument required\n", fmlsh_string, nstr);
  }

  char* commands[21];
  int len;

  if ((len = get_history(root, commands)) < n) {
    n = len;
  }
  for (int i = 0; i < n; i++) {
    printf("%s", commands[i + len - n]);
  }
}
