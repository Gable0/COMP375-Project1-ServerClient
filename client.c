/**
 * client.c
 *
 * @author Your name here
 *
 * USD COMP 375: Computer Networks
 * Project 1
 *
 * @todo: Add file description here.
 */

#define _XOPEN_SOURCE 600

#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include "io_helpers.h"

/**
 * Type that represents the possible actions that a user could select while running
 * the program.
 */
typedef enum RequestType {
	TEMPERATURE = 1,
	HUMIDITY = 2,
	WIND_SPEED = 3,
	QUIT = 4,
	INVALID
} RequestType;

RequestType get_user_request();

void main_loop();

int main() {
	main_loop();
	return 0;
}

/**
 * Loop to keep asking user what they want to do and calling the appropriate
 * function to handle the selection.
 */
void main_loop() {
	int server_fd = connect_to_host("hopper.sandiego.edu", "FIXME"); // @fixme No idea if this is the right place for this

	bool done = false;
	while (!done) {
		RequestType selection = get_user_request();

		switch (selection) {
			case TEMPERATURE:
				// TODO: Handle case one by calling a function you write
				break;
			case HUMIDITY:
				// TODO: Handle case one by calling a function you write
				break;
			case WIND_SPEED:
				// TODO: Handle case one by calling a function you write
				break;
			case QUIT:
				done = true;
				break;
			default:
				fprintf(stderr, "ERROR: Invalid selection\n");
				break;
		}
	}

	close(server_fd);
}

/** 
 * Print command prompt to user and obtain user input.
 *
 * @return The user's desired selection, or -1 if invalid selection.
 */
RequestType get_user_request() {
	// TODO: add printfs to print out the options

	// Read in a value from standard input
	char input[10];
    get_text_input(input, 10, stdin);

    // get rid of newline, if there is one
	char *new_line = strchr(input, '\n');
	if (new_line != NULL) new_line[0] = '\0';

	// convert string to a long int
	char *end;
	long selection = strtol(input, &end, 10);

	if (end == input || *end != '\0') {
		// input was not an integer
		return INVALID;
	}
	else if (selection < 1 || selection > 4) {
		// input wasn't in the valid range
		return INVALID;
	}
	else {
		return selection;
	}
}


