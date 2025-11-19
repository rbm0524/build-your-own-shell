#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>
#define TRUE 1
#define FALSE 0

char *commandTable[] = {"exit", "echo", "type", "pwd","cd"};

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

    memset(pathBuffer, 0, sizeof(pathBuffer));

    while(dir != NULL) {
      int charCount = snprintf(pathBuffer, sizeof(pathBuffer), "%s/%s", dir, program);
      if(!(access(pathBuffer, X_OK))) {
        char currentWorkingDir[PATH_MAX];
        getcwd(currentWorkingDir, PATH_MAX);
        chdir(pathBuffer);

        if (args != NULL) {
          snprintf(pathBuffer, sizeof(pathBuffer), "%s %s", program, args);
        }

        FILE *fp = popen(pathBuffer, "r");
        char buffer[512] = {0};
    
        if(fp == NULL) {
          return FALSE;        
        }
    
        while(fgets(buffer, sizeof(buffer), fp) != NULL) {
          printf("%s", buffer);
        }
        
        pclose(fp);
        
        free(copyEnvPath);
        chdir(currentWorkingDir);
        return TRUE;
      }

      dir = strtok(NULL, ":");
      memset(pathBuffer, 0, sizeof(pathBuffer));
    }

    return FALSE;
}

void builtin_pwd() {
  char currentWorkingDir[PATH_MAX];
  getcwd(currentWorkingDir, PATH_MAX);
  printf("%s\n", currentWorkingDir);
}

void builtin_cd(char *args) {
  char moveWorkingDir[PATH_MAX];
  char * homePath = getenv("HOME");
  char *tokenized = strtok(args, "/");
  int charCount = 0;

  // 시작 글자가 /이면 
  if(*args =='/') {
    moveWorkingDir[0] = '/';
    charCount++;
  }

  while(tokenized != NULL) {
    if(strcmp(tokenized , "~") == 0) {
      charCount += sprintf(moveWorkingDir + charCount, "%s/", homePath); // homepath는 /usr/home 이렇게 들어가니까
    } else {
      charCount += sprintf(moveWorkingDir + charCount, "%s/", tokenized);
    }
    tokenized = strtok(NULL, "/");
  }

  if(access(moveWorkingDir, X_OK) == 0) {
    chdir(moveWorkingDir);
  } else {
    printf("cd: %s: No such file or directory\n", args);
  }

}