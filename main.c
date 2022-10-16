#include "bg.h"
#include "cd.h"
#include "command.h"
#include "discover.h"
#include "fg.h"
#include "headers.h"
#include "history.h"
#include "jobs.h"
#include "ls.h"
#include "pinfo.h"
#include "prompt.h"
#include "runner.h"

char cwd[PATH_MAX], prevwd[PATH_MAX], curpath[PATH_MAX];
char* root;
char errstr[1000];
int process_pids[100] = {0};
char* process_commands[100];
int process_time = 0;
int fg_process = 0;
int fg_pid = 0;

char* FML_string =
    "\033[01;34;40m"
    "F" CLR RESET
    "\033[1;36;40m"
    "M" CLR RESET
    "\033[1;32;40m"
    "L" CLR RESET
    "\033[1;33;40m"
    " SH" CLR RESET
    "\033[1;35;40m"
    "E" CLR RESET
    "\033[1;31;40m"
    "LL" CLR RESET;

char* fmlsh_string =
    "\033[01;34;40m"
    "f" CLR RESET
    "\033[1;36;40m"
    "m" CLR RESET
    "\033[1;32;40m"
    "l" CLR RESET
    "\033[1;33;40m"
    "s" CLR RESET
    "\033[1;31;40m"
    "h" CLR RESET;

void add_process(int pid, char* command, char* root2) {
  if (!pid) return;

  int pos = 1;

  while (process_pids[pos]) pos++;
  printf("[%d] %d\n", pos, pid);

  free(root);
  root = malloc(sizeof(char) * strlen(root2));
  strcpy(root, root2);

  process_pids[pos] = pid;
  process_commands[pos] = malloc(strlen(command) + 1);
  strcpy(process_commands[pos], command);
}

void process_handler() {
  int st, pid;

  if ((pid = waitpid(-1, &st, WNOHANG)) <= 0) {
    return;
  }

  int pos = 0;
  while (process_pids[pos++] != pid)
    ;
  pos--;

  if (WIFSTOPPED(st)) {
    return;
  }

  fprintf(stderr, "[%d] %s with pid = %d exited %s\n", pos, process_commands[pos],
          process_pids[pos], WIFEXITED(st) ? "normally" : "abnormally");

  prompt(root, cwd);

  process_pids[pos] = 0;
  free(process_commands[pos]);
  process_commands[pos] = NULL;
}

void handle_c() {
  if (fg_process != 0) {
    fg_process = 0;
    process_time = 0;
  } else {
    printf("\n");
    prompt(root, cwd);
  }
}

void sig(int jobn, int sign, int nargs) {
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
  free(process_commands[jobn]);
  process_commands[jobn] = NULL;
}
// void handle_z() {
//   if (kill(pid, SIGCONT) < 0) {
//     sprintf(errstr, "%s: Error");
//     perror(errstr);
//   }
// }

int main() {
  signal(SIGCHLD, process_handler);

  char root[PATH_MAX], path[PATH_MAX];
  if (getcwd(root, sizeof(root)) == NULL) {
    perror("getcwd() error");
    exit(1);
  }
  strcpy(cwd, root);
  strcpy(prevwd, root);

  printf("\n%s" CLR BOLD MAG "\nSriteja Reddy Pashya" CLR BOLD YEL " |" CLR BOLD CYN " 2021111019" CLR RESET "\n",
         FML_string);
  printf("\n");

  signal(SIGINT, handle_c);
  // signal(SIGTSTP, handle_z);

  char* commands[200][500];
  char* subcommands[200][500];
  char command[1000];
  int no_com;
  int stdin_dup, stdout_dup;
  int pipe_files[1000][2];
  while (1) {
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
      perror("getcwd error");
      exit(1);
    }
    prompt(root, cwd);
    no_com = get_commands(command, commands);

    add_history(root, command);

    for (int i = 0; i < no_com; i++) {
      int background = 0;

      if (commands[i][0] == NULL) continue;

      if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd error");
        exit(1);
      }

      int nargs1 = 0;
      while (commands[i][nargs1++] != NULL)
        ;
      nargs1--;
      if (nargs1 == 0) {
        perror("get_commands() error: no argument");
      }
      if (nargs1 >= 1 && !strcmp(commands[i][nargs1 - 1], "&")) {
        free(commands[i][nargs1 - 1]);
        commands[i][nargs1 - 1] = NULL;
        background = 1;
        nargs1--;
      }

      // printf("nargs1: %d\n", nargs1);

      int npipes = 0;
      int cur_pipe = 0, cur_arg = 0;
      int err_flag = 0;
      for (int j = 0; j < nargs1; j++) {
        if (!strcmp(commands[i][j], "|")) {
          subcommands[npipes++][cur_arg] = NULL;
          if (cur_arg == 0) {
            fprintf(stderr, "%s: command empty between pipes\n", fmlsh_string);
            err_flag = 1;
            break;
          }
          cur_arg = 0;
          continue;
        }
        subcommands[npipes][cur_arg] = (char*)malloc(sizeof(char) * (strlen(commands[i][j]) + 1));
        strcpy(subcommands[npipes][cur_arg++], commands[i][j]);
      }
      subcommands[npipes++][cur_arg] = NULL;
      if (!strcmp(commands[i][nargs1 - 1], "|")) {
        fprintf(stderr, "%s: command empty after last pipe\n", fmlsh_string);
        err_flag = 1;
      }

      for (int j = 0; commands[i][j] != NULL; j++) {  // free
        free(commands[i][j]);
      }

      if (err_flag) {
        for (int _i = 0; _i < npipes; _i++) {
          for (int j = 0; subcommands[_i][j] != NULL; j++) {
            free(subcommands[_i][j]);
          }
        }
        continue;
      }

      for (int _i = 0; _i < npipes; _i++) {
        int input_mode = 0, output_mode = 0;
        int input_fd = 0, output_fd = 0;

        int nargs = 0;
        while (subcommands[_i][nargs++] != NULL)
          ;
        nargs--;

        if (_i < npipes - 1) {
          if (pipe(pipe_files[_i]) < 0) {
            sprintf(errstr, "%s: Error", fmlsh_string);
            perror(errstr);
            break;
          }
          output_fd = pipe_files[_i][1];
          // output_mode = 1;
        }
        if (_i > 0) {
          input_fd = pipe_files[_i - 1][0];
          // input_mode = 1;
        }

        if (_i == 0 || _i == npipes - 1) {
          int redirect_flag = 0;
          int temp_nargs = nargs;
          int error_flag = 0, error_flag2 = 0;
          char temp_path[PATH_MAX];
          for (int j = 1; j < temp_nargs; j++) {
            if (!strcmp(subcommands[_i][j], "<") || !strcmp(subcommands[_i][j], ">") || !strcmp(subcommands[_i][j], ">>")) {
              if (redirect_flag == 0) {
                nargs = j;
                redirect_flag = 1;
              }
              if (j + 1 == temp_nargs) {
                fprintf(stderr, "%s: No file specified for operator %s\n", fmlsh_string, subcommands[_i][j]);
                error_flag2 = 1;
                break;
              }
              strcpy(temp_path, subcommands[_i][j + 1]);
              if (!strncmp(subcommands[_i][j + 1], "~", 1)) {
                sprintf(temp_path, "%s%s", root, subcommands[_i][j + 1] + 1);
              }
              if (!strcmp(subcommands[_i][j], "<")) {
                input_mode = 1;
                input_fd = open(temp_path, O_RDONLY);
                if (input_fd < 0) {
                  sprintf(errstr, "%s: \'%s\'", fmlsh_string, subcommands[_i][j + 1]);
                  error_flag = 1;
                  break;
                }
              } else if (!strcmp(subcommands[_i][j], ">")) {
                output_mode = 1;
                output_fd = open(temp_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
              } else if (!strcmp(subcommands[_i][j], ">>")) {
                output_mode = 2;
                output_fd = open(temp_path, O_WRONLY | O_CREAT | O_APPEND, 0644);
              }

              free(subcommands[_i][j]);
              subcommands[_i][j] = NULL;
              free(subcommands[_i][j + 1]);
              subcommands[_i][j + 1] = NULL;

              j++;
            } else if (redirect_flag) {
              for (int k = j; k < nargs; k++) {
                free(subcommands[_i][k]);
              }
              fprintf(stderr, "%s: \"%s\" is not an i/o operator\n", fmlsh_string, subcommands[_i][j]);
              error_flag2 = 1;
              break;
            }
          }

          if (error_flag2) {
            continue;
          }

          if (output_fd < 0 || error_flag) {
            if (!error_flag) sprintf(errstr, "%s: Error", fmlsh_string);
            perror(errstr);
            for (int j = 0; subcommands[_i][j] != NULL; j++) {
              free(subcommands[_i][j]);
            }
            continue;
          }
        }

        if (input_mode || _i > 0) {
          stdin_dup = dup(STDIN_FILENO);
          dup2(input_fd, STDIN_FILENO);
        }
        if (output_mode || _i < npipes - 1) {
          stdout_dup = dup(STDOUT_FILENO);
          dup2(output_fd, STDOUT_FILENO);
        }

        if (!strcmp(subcommands[_i][0], "cd")) {
          fflush(stdout);
          if (nargs == 1) {
            strcpy(prevwd, cwd);
            if (chdir(root) < 0) {
              perror("Error: cd: root");
              // exit(0);
            }
          } else if (nargs > 2) {
            printf("%s: cd: Too many arguments", fmlsh_string);
          } else {
            strcpy(curpath, cwd);
            if (cd(root, cwd, subcommands[_i][1], prevwd) < 0) {
              sprintf(errstr, "%s: cd: %s", fmlsh_string, subcommands[_i][1]);
              perror(errstr);
            } else {
              strcpy(prevwd, curpath);
            }
          }
        } else if (!strcmp(subcommands[_i][0], "pwd")) {
          printf("%s\n", cwd);
        } else if (!strcmp(subcommands[_i][0], "echo")) {
          for (int j = 1; j < nargs; j++)
            printf("%s ", subcommands[_i][j]);
          printf("\n");
        } else if (!strcmp(subcommands[_i][0], "ls")) {
          ls(root, subcommands[_i], nargs);
        } else if (!strcmp(subcommands[_i][0], "history")) {
          if (nargs > 2) {
            sprintf(errstr, "%s: history: Too many arguments\n", fmlsh_string);
            perror(errstr);
          } else {
            history(root, subcommands[_i][1]);
          }
        } else if (!strcmp(subcommands[_i][0], "pinfo")) {
          pinfo(root, (nargs - 1) ? subcommands[_i][1] : "self");
        } else if (!strcmp(subcommands[_i][0], "discover")) {
          discover(root, subcommands[_i], nargs);
        } else if (!strcmp(subcommands[_i][0], "jobs")) {
          if (nargs > 2) {
            fprintf(stderr, "%s: jobs: Too many arguments\n", fmlsh_string);
          } else {
            jobs(process_pids, process_commands, subcommands[_i][1], nargs);
          }
        } else if (!strcmp(subcommands[_i][0], "bg")) {
          bg(atoi(subcommands[_i][1]), nargs, process_pids);
        } else if (!strcmp(subcommands[_i][0], "fg")) {
          fg(atoi(subcommands[_i][1]), nargs, process_pids, process_commands);
        } else if (!strcmp(subcommands[_i][0], "sig")) {
          printf("%s, %s\n", subcommands[_i][1], subcommands[_i][2]);
          if (atoi(subcommands[_i][1]) > 99 || atoi(subcommands[_i][1]) < 1) {
            fprintf(stderr, "%s must be between 1 and 99", subcommands[_i][1]);
          } else {
            kill(atoi(subcommands[_i][1]), atoi(subcommands[_i][2]));
            process_pids[atoi(subcommands[_i][1])] = 0;
            free(process_commands[atoi(subcommands[_i][1])]);
            process_commands[atoi(subcommands[_i][1])] = NULL;
            // process_handler();
          }

        } else if (!strcmp(subcommands[_i][0], "exit")) {
          exit(0);
        } else if (!strcmp(subcommands[_i][0], "fml")) {
          printf("%s: You're Welcome\n", fmlsh_string);
        } else {
          clock_t t1, t2;

          t1 = clock();
          // printf("t1 = %ld\n", t1);
          if (background == 0) fg_process = 1;
          int pid = run_command(background, subcommands[_i]);
          fg_process = 0;
          t2 = clock();
          // printf("t2 = %ld\n", t2);

          if (!strcmp(subcommands[_i][0], "sleep") && background == 0) {
            process_time = atoi(subcommands[_i][1]);
          }
          // printf("sleep %d\n", process_time);
          double time_taken = ((double)(t2 - t1)) / CLOCKS_PER_SEC;
          // printf("time = %f\n", time_taken);

          if (pid > 0 && background) {
            add_process(pid, subcommands[_i][0], root);
          }
        }

        if (input_mode || _i > 0) {
          close(input_fd);
          dup2(stdin_dup, STDIN_FILENO);
          close(stdin_dup);
        }
        if (output_mode || _i < npipes - 1) {
          close(output_fd);
          dup2(stdout_dup, STDOUT_FILENO);
          close(stdout_dup);
        }

        if (_i < npipes - 1) close(pipe_files[_i][1]);
        if (_i > 0) close(pipe_files[i - 1][0]);

        for (int j = 0; subcommands[_i][j] != NULL; j++) {
          free(subcommands[_i][j]);
        }
      }
    }
  }
}
