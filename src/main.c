#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include "builtin.h"

void builtin_exit(char *args);
void builtin_echo(char *args);
void builtin_type(char *args, char* envPath);

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);
  char * envPath = getenv("PATH");

  while (1) {
    printf("$ ");
    char command[512];
    fgets(command, sizeof(command), stdin);
    command[strcspn(command, "\n")] = '\0';

    char *cmd = strtok(command, " ");
    char *args = strtok(NULL, "\0"); // 나머지 전체를 인자로 취급

    // 명령어 해석하기
    if(strstr(cmd, "exit") != NULL) { // 문자열이 서로 같으면 0을 리턴한다.
      builtin_exit(args);
    } else if (strstr(cmd, "echo") != NULL) {
      builtin_echo(args);
    } else if(strstr(cmd, "type") != NULL) {
      builtin_type(envPath, args);
    } else {
      printf("%s: command not found\n", cmd);
    }
  }

  return 0;
}
