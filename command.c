#include "command.h"

#include "headers.h"

int get_commands(char* command, char* commands[200][500]) {
  char* str = NULL;
  unsigned long size = 1000;
  int len = getline(&str, &size, stdin);
  if (len < 0) {
    printf("\n");
    exit(0);
  }
  strcpy(command, str);

  str[len - 1] = '\0';

  if (str[0] == '\0') return 0;

  int white = 1;            // whitespace or middle of word
  int a = 0, b = 0, c = 0;  // command, arg, char;
  char temp[1000];

  for (int i = 0; i < len; i++) {
    if (isspace(str[i]) || str[i] == ';' || str[i] == '\0' || str[i] == '&') {
      if (white == 0) {
        temp[c] = '\0';
        commands[a][b] = malloc(sizeof(char) * (strlen(temp) + 2));
        strcpy(commands[a][b], temp);
        b++;
        c = 0;
        temp[0] = '\0';
        white = 1;
      }
      if ((str[i] == ';' || str[i] == '\0' || str[i] == '&') && b != 0) {
        if (str[i] == '&') {
          commands[a][b] = malloc(sizeof(char) * 3);
          strcpy(commands[a][b], "&");
          b++;
        }
        commands[a][b] = NULL;
        b = 0;
        a++;
        white = 1;
      }
    } else {
      white = 0;
      temp[c] = str[i];
      c++;
    }
  }

  if (b != 0 || c != 0) {
    perror("Error taking in commands");
    exit(0);
  }

  return a;
}
