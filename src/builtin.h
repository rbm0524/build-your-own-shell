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

void builtin_type(char *args) {
    for(int i = 0; i < sizeof(commandTable)/sizeof(char*); i++) {
        if(strcmp(args, commandTable[i])) {
            printf("%s is a shell builtin\n", args);
            return;
        }
    }
    printf("%s: not found\n", args);
}