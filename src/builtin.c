#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>
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
    char cmpBuffer[512];

    memset(pathBuffer, 0, sizeof(pathBuffer));
    memset(cmpBuffer, 0, sizeof(cmpBuffer));

    while(dir != NULL) {
      int charCount = snprintf(pathBuffer, sizeof(pathBuffer), "%s/%s", dir, program);
      if(!(access(pathBuffer, X_OK))) {
        char *currentWorkingDir;
        getcwd(currentWorkingDir, PATH_MAX);
        chdir(pathBuffer);

        if (args != NULL) {
          snprintf(pathBuffer, sizeof(pathBuffer), "%s %s", program, args);
        }

        free(copyEnvPath);
        chdir(currentWorkingDir);
        break;
      }
      dir = strtok(NULL, ":");
      memset(pathBuffer, 0, sizeof(pathBuffer));
    }

    if(memcmp(pathBuffer, cmpBuffer, 512) != 0) {
      FILE *fp = popen(pathBuffer, "r");
      char buffer[512] = {0};

      if(fp == NULL) {
        return FALSE;        
      }

      while(fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("%s\n", buffer);
        memset(buffer, 0, sizeof(buffer));
      }
      
      pclose(fp);
      return TRUE;
    }

    return FALSE;
}