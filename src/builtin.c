#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#define TRUE 1
#define FALSE 0

char *commandTable[] = {"exit", "echo", "type"};

void builtin_exit(char *args) {
  if (args != NULL) {
    int exit_code = atoi(args);
    exit(exit_code);
  } else {
    exit(0);
  }
}

void builtin_echo(char *args) {
  if (args != NULL) {
    printf("%s\n", args);
  } else {
    printf("\n");
  }
}

void builtin_type(char *envPath, char *args) {
    
    for(int i = 0; i < sizeof(commandTable)/sizeof(char*); i++) {
        if(!strcmp(args, commandTable[i])) {
            printf("%s is a shell builtin\n", args);
            return;
        }
    }
    
    char *copyEnvPath = strdup(envPath); // malloc을 호출하여 string의 사본에 대한 포인터를 반환
    char *dir = strtok(copyEnvPath, ":");
    while(dir != NULL) {
      char pathBuffer[512];
      snprintf(pathBuffer, sizeof(pathBuffer), "%s/%s", dir, args);
      if(!(access(pathBuffer, X_OK))) {
        printf("%s is %s\n", args, pathBuffer);
        free(copyEnvPath);
        return;
      }
      dir = strtok(NULL, ":");
    }

    printf("%s: not found\n", args);
    free(copyEnvPath);
}

int executeProgram(char *envPath, char *program, char * args) {
    char *copyEnvPath = strdup(envPath); // malloc을 호출하여 string의 사본에 대한 포인터를 반환
    char *dir = strtok(copyEnvPath, ":");
    char pathBuffer[512];
    while(dir != NULL) {
      snprintf(pathBuffer, sizeof(pathBuffer), "%s/%s", program, args);
      if(!(access(pathBuffer, X_OK))) {
        printf("%s is %s\n", args, pathBuffer);
        free(copyEnvPath);
        break;
      }
      dir = strtok(NULL, ":");
      memset(pathBuffer, 0, sizeof(pathBuffer));
    }

    if(*(pathBuffer) != 0) {
      snprintf(pathBuffer, sizeof(pathBuffer), "%s %s", program, args);
      system(pathBuffer);
      return TRUE;
    }

    return FALSE;

}