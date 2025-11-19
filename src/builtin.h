#pragma once

void builtin_exit(char *args);
void builtin_echo(char *args);
void builtin_type(char *envPath, char *args);
int executeProgram(char *envPath, char *program, char * args);
void builtin_pwd();