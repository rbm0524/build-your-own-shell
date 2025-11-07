#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);

  while (1) {
    printf("$ ");
    char command[1024];
    fgets(command, sizeof(command), stdin);
    command[strcspn(command, "\n")] = '\0';

    if(strstr(command, "exit") != NULL) { // 문자열이 서로 같으면 0을 리턴한다.
      if(command[strcspn(command, "\0") - 1] == '0') {
        exit(0);
      } else {
        exit(1);
      }
    } else if (strstr(command, "echo") != NULL) {
      printf("%s\n", strstr(command, "echo") + 5);
    } else {
      printf("%s: command not found\n", command);
    }
  }

  return 0;
}