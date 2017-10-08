#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_LENGTH 256
#define MIN_MENU_INPUT 1

static char receive[BUFFER_LENGTH];
char *trim_s(char *string);
void get_input(char *string, int length);

