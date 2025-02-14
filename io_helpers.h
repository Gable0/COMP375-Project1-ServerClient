#ifndef IO_HELPERS_H
#define IO_HELPERS_H

#include <stdio.h>

void get_text_input(char* buffer, size_t buffer_size, FILE* input_file);
int connect_to_host(char *hostname, char *port);

#endif