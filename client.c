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
void get_text_input(char* buffer, size_t buffer_size);
int connect_to_host(char *hostname, char *port);
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
	int server_fd = connect_to_host("hopper.sandiego.edu", "FIXME"); // I hope this is the right place for this...

	while (true) {
		RequestType selection = get_user_request();

		switch (selection) {
			case 1:
				// TODO: Handle case one by calling a function you write
				break;
			// TODO: add cases for other menu options
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
    get_text_input(input, 10);

    // get rid of newline, if there is one
	char *new_line = strchr(input, '\n');
	if (new_line != NULL) new_line[0] = '\0';

	// convert string to a long int
	char *end;
	long selection = strtol(input, &end, 10);

	if (end == input || *end != '\0') {
		return INVALID;
	}

	return selection;
}

/**
 * Reads input into the specified buffer, doing error checking alone the way.
 * 
 * @param buffer The place where input will be stores
 * @param buffer_size The size (in bytes) of the buffer
 */
void get_text_input(char* buffer, size_t buffer_size) {
    memset(buffer, 0, buffer_size); // set all characters in input to '\0' (i.e. nul)
    char *read_str = fgets(buffer, buffer_size, stdin);

    // Check if EOF or an error, exiting the program in both cases.
    if (read_str == NULL)
    {
        if (feof(stdin))
        {
            exit(0);
        }
        else if (ferror(stdin))
        {
            perror("fgets");
            exit(1);
        }
    }
}

/**
 * Socket implementation of connecting to a host at a specific port.
 *
 * @param hostname The name of the host to connect to (e.g. "foo.sandiego.edu")
 * @param port The port number to connect to
 * @return File descriptor of new socket to use.
 */
int connect_to_host(char *hostname, char *port) {
	// Step 1: fill in the address info in preparation for setting 
	//   up the socket
	int status;
	struct addrinfo hints;
	struct addrinfo *servinfo;  // will point to the results

	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_INET;       // Use IPv4
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

	// get ready to connect
	if ((status = getaddrinfo(hostname, port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		exit(1);
	}

	// Step 2: Make a call to socket
	int fd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	if (fd == -1) {
		perror("socket");
		exit(1);
	}

	// Step 3: connect!
	if (connect(fd, servinfo->ai_addr, servinfo->ai_addrlen) != 0) {
		perror("connect");
		exit(1);
	}

	freeaddrinfo(servinfo); // free's the memory allocated by getaddrinfo

	return fd;
}
