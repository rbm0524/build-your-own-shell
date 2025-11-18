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
    char *cmdPath[] = strtok(envPath, ":");
    
    for(int i = 0; i < sizeof(commandTable)/sizeof(char*); i++) {
        if(!strcmp(args, commandTable[i])) {
            printf("%s is %s\n", args, envPath);
            return;
        }
    }
    
    for(int i = 0; i < sizeof(cmdPath) / sizeof(char*); i++) {
      DIR *dir = opendir(cmdPath[i]);
      if(dir == NULL) continue;
      while(readdir(dir) != NULL) {
          printf("%s is %s\n", args, dir);
          return;
      }
    }

    printf("%s: not found\n", args);
}