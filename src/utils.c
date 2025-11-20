#include <string.h>

void ltrim(char *string) {
    while(*string == ' ') {
        string++;
    }
}
void rtrim(char *string) {
    int length = strlen(string);
    while(*(string + length) == ' ') {
        length--;
    }
    if(length > 0) {
        *(string + length + 1) = '\0';
    }
}