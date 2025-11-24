#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>
#include "utils.h"
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
  int i = 0;
  int in_single_quote = 0;
  int in_double_quote = 0;
  FILE *buffer = stdout;
  FILE *errorFile = NULL;

  char *existRedirect = strstr(args, ">");
  char *redirectPath = existRedirect == NULL ? NULL : existRedirect + 1;
  if(existRedirect != NULL) {
    *existRedirect = '\0';
    ltrim(&redirectPath);
    rtrim(redirectPath);

    if(*(existRedirect - 1) == '1') {
      *(existRedirect - 1) = ' ';
    } else if(*(existRedirect - 1) == '2') {
      *(existRedirect - 1) = ' ';
      ltrim(&args);
      rtrim(args);
      errorFile = fopen(redirectPath, "w");
    } else {
      ltrim(&args);
      rtrim(args);
      buffer = fopen(redirectPath, "w");
    }
    

  }

  while(*(args+i) != '\0') {

    if(!in_single_quote && !in_double_quote && *(args+i) == '\\') {
      // printf("%c", *(args + i + 1));
      fprintf(buffer, "%c", *(args + i + 1));
      i += 2;
      continue;
    }
    
    if(in_double_quote) {
      if(*(args+i) != '\"') {
        if(*(args+i) == '\\') {
          // printf("%c", *(args + i + 1));
          fprintf(buffer, "%c", *(args + i + 1));
          i += 2;
          continue;
        } else {
          //printf("%c", *(args + i));
          fprintf(buffer, "%c", *(args + i));
        }
      }
    } else {
      if(in_single_quote) {
        if(*(args+i) != '\'') fprintf(buffer, "%c", *(args + i)); //printf("%c", *(args + i));
      } else {
        if((*(args+i-1) != *(args+i) && *(args+i) == ' ') || (*(args+i) != ' ' && *(args+i) != '\"' && *(args+i) != '\'')) {
          // printf("%c", *(args + i));
          fprintf(buffer, "%c", *(args + i));
        }
      }
    }

    if (ferror(buffer) && errorFile != NULL) {
      fprintf(errorFile, "echo: write error\n"); // 2>로 지정된 파일에 에러 씀
    }
    
    if(*(args+i) == '\'' && !in_double_quote) {
      in_single_quote = !in_single_quote;
    }
    
    if(*(args+i) == '\"' && !in_single_quote) {
      in_double_quote = !in_double_quote;
    }
    
    i++;
  }
  
  fprintf(buffer, "%s", "\n"); // 커서가 맨 끝으로 이동한다.
  // rewind(buffer); // 되감기

  if(buffer != stdout) {
    fclose(buffer);
  }

  if(errorFile != NULL) {
    fclose(errorFile); // 2> 로 연 파일 닫기
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
  FILE *printbuffer = stdout;

    memset(pathBuffer, 0, sizeof(pathBuffer));

    while(dir != NULL) {
      snprintf(pathBuffer, sizeof(pathBuffer), "%s/%s", dir, program);
      if(!(access(pathBuffer, X_OK))) {
        char currentWorkingDir[PATH_MAX];
        getcwd(currentWorkingDir, PATH_MAX);
        chdir(pathBuffer);
        
        if (args != NULL) {
        /*
          char *existRedirect = strstr(args, ">");
          char *redirectPath = existRedirect == NULL ? NULL : existRedirect + 1;
          if(existRedirect != NULL) {
            *existRedirect = '\0';
            *(existRedirect - 1) = ' ';
            /*
            if(*(existRedirect - 1) == '2') {
              printbuffer = stderr;
            }
              
            ltrim(&redirectPath);
            rtrim(redirectPath);
            ltrim(&args);
            rtrim(args);
            if(printbuffer == stdout) printbuffer = fopen(redirectPath, "a+");
          }
          snprintf(pathBuffer, sizeof(pathBuffer), "%s %s", program, args);
          */
          snprintf(pathBuffer, sizeof(pathBuffer), "%s %s", program, args);
        }

        if(strstr(args, "2>")) {

        }
        
        FILE *fp = popen(pathBuffer, "r"); // 명령과 모드, 실행 결과는 fp에 저장된다.
        
        if(fp == NULL) {
          return FALSE;        
        }
        
        char copybuffer[512];
        char *totalBuffer = (char*)malloc(sizeof(copybuffer));
        while(fgets(copybuffer, sizeof(copybuffer), fp) != NULL){
          /*
          if(printbuffer == stderr) {
            continue;
            }
            */
            //fprintf(printbuffer, "%s", copybuffer);
            strcat(totalBuffer, copybuffer);
            
            // printf("%s", copybuffer);
          }
          
          int exitCode = pclose(fp);

          if (exitCode == 0) {
            fprintf(printbuffer, "%s", totalBuffer);
          }
          if(printbuffer != stdout) {
            fclose(printbuffer);
          }
          
        
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