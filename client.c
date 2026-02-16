/**
 * client.c
 *
 * @author Gable Krich
 *
 * USD COMP 375: Computer Networks
 * Project 1
 *
 * Description: A client that connects to a sensor network to get weather data 
 */

#define _XOPEN_SOURCE 600
#define BUFF_SIZE 1024

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

// forward declarations (yay for archaic C rules)
RequestType get_user_request();
void handle_request(RequestType request);
void main_loop();
int get_server_info(char *hostname, int *port, char *password);
int authenticate_weather_server(char *hostname, int port, char *password);
void get_and_display_sensor_data(int weather_fd, RequestType request);
void close_weather_connection(int weather_fd);


/**
 * Main function.
 *
 * @return Exit status
 */
int main() {
	main_loop();
	return 0;
}


/**
 * Loop to keep asking user what they want to do and calling the appropriate
 * function to handle the selection.
 */
void main_loop() {
	printf("WELCOME TO THE COMP375 SENSOR NETWORK\n");
	bool done = false;
	while (!done) {
		RequestType selection = get_user_request();

		switch (selection) {
			case TEMPERATURE:
			case HUMIDITY:
			case WIND_SPEED:
				handle_request(selection); // @todo: fix the second parameter (DONE)
				break;
			case QUIT:
				done = true;
				break;
			default:
				fprintf(stderr, "ERROR: Invalid selection\n");
				break;
		}
	}
	printf("GOODBYE!\n");
}

/** 
 * Print command prompt to user and obtain user input.
 *
 * @return The user's desired selection, or -1 if invalid selection.
 */
RequestType get_user_request() {
	// @todo add printfs to print out the options (DONE) 
	printf("Which sensor would you like to read: \n");
	printf("\t(1) Air temperature\n");
	printf("\t(2) Relative humidity\n");
	printf("\t(3) Wind speed\n");
	printf("\t(4) Quit Program\n");
	printf("Selection: ");


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

/**
 * Get the weather server info from hopper.
 *
 * @param hostname Where to store the hostname
 * @param port Where to store the port
 * @param password Where to store the password
 * @return 0 if success, -1 if error
 */
int get_server_info(char *hostname, int *port, char *password) {
	//connecting to hopper/sending the auth message
	int hopper_fd = connect_to_host("hopper.sandiego.edu", "7030"); // connect to hopper

	//make sure the connection was made (GIVEN BY CUSTOM GEM MODEL)
	if (hopper_fd < 0) {
		fprintf(stderr, "Failed to connect to hopper.sandiego.edu\n");
		return -1; // Exit handle_request early
}

	char message[BUFF_SIZE]; //make the user input 
	memset(message, 0, BUFF_SIZE); //reset the user input with all 0's
	strcpy(message, "AUTH password123\n");
	if (send(hopper_fd, message, strlen(message), 0) < 0) {
		close(hopper_fd);
		return -1;
	}

	//receving the message from hopper
	char response[BUFF_SIZE];
	memset(response, 0, BUFF_SIZE);

	//(GOT FROM CUSTOM GEM MODEL) makes sure that there is a null terminator at the end so the sscan doesn't bug
	int bytes = recv(hopper_fd, response, BUFF_SIZE - 1, 0); 
	if (bytes <= 0) {
		close(hopper_fd);
		return -1;
	}
	response[bytes] = '\0'; // force a null terminator at the end of what was received

	if (sscanf(response, "CONNECT %s %d %s", hostname, port, password) != 3) {
		close(hopper_fd);
		return -1;
	}

	close(hopper_fd); //close the connection with hopper
	return 0;
}

/**
 * Connect to weather server and authenticate.
 *
 * @param hostname Weather server hostname
 * @param port Weather server port
 * @param password Password to use
 * @return Socket fd if success, -1 if error
 */
int authenticate_weather_server(char *hostname, int port, char *password) {
	//connecting to the weather server
	char port_str[10]; 
	sprintf(port_str, "%d", port); //convert the int port to a string (GOT THIS FROM CUSTOM GEM MODEL)

	int weather_fd = connect_to_host(hostname, port_str);

	//make sure the connection was made (GIVEN BY CUSTOM GEM MODEL)
	if (weather_fd < 0) {
		fprintf(stderr, "Failed to connect to weather station\n");
		return -1; // Exit handle_request early
	}

	char auth_msg[BUFF_SIZE];
	sprintf(auth_msg, "AUTH %s\n", password); //put the password variable from the response, and put it in auth_msg

	if (send(weather_fd, auth_msg, strlen(auth_msg), 0) < 0) {
		close(weather_fd);
		return -1;
	}

	char weather_auth_response[BUFF_SIZE];
	int weather_bytes = recv(weather_fd, weather_auth_response, BUFF_SIZE - 1, 0); //listen for the SUCCESS\n or not message from the server
	if (weather_bytes <= 0) {
		close(weather_fd);
		return -1;
	}
	weather_auth_response[weather_bytes] = '\0'; // force a null terminator at the end of what was received 

	return weather_fd;
}

/**
 * Send query and display sensor data.
 *
 * @param weather_fd Socket for weather server
 * @param request Type of sensor data to get
 */
void get_and_display_sensor_data(int weather_fd, RequestType request) {
	//setting user query (THIS IS WHERE YOU LEFT OFF, YOU NEED TO PUT QUERY IN EACH OF THE SWITCH STATEMENTS NOW)
	char *query = NULL;

	switch (request) {
		case TEMPERATURE:
			// @todo: put any code specific to this case.
			query = "AIR TEMPERATURE\n";
			break;
		case HUMIDITY:
			// @todo: put any code specific to this case.
			query = "RELATIVE HUMIDITY\n";
			break;
		case WIND_SPEED:
			// @todo: put any code specific to this case.
			query = "WIND SPEED\n";
			break;
		default:
			fprintf(stderr, "Invalid request type\n");
			exit(1);
	}

	if (query != NULL) {
    send(weather_fd, query, strlen(query), 0);

    char response[BUFF_SIZE];
    memset(response, 0, BUFF_SIZE);

    int data_bytes = recv(weather_fd, response, BUFF_SIZE - 1, 0);

    if (data_bytes > 0) {
        response[data_bytes] = '\0';
    }
	//variables
    time_t timestamp;
    int value;
    char unit[10];

	//parse through the servers response
    sscanf(response, "%ld %d %s", &timestamp, &value, unit);

    char *time_str = ctime(&timestamp);
    char *new_time_str = strchr(time_str, '\n');
    if (new_time_str != NULL) new_time_str[0] = '\0';

    //NULL terminate
    char sensor_name[50];
    strcpy(sensor_name, query);
    char *nl = strchr(sensor_name, '\n');
    if (nl != NULL) nl[0] = '\0';

    printf("The last %s reading was %d %s, taken at %s\n", sensor_name, value, unit, time_str);
}
}

/**
 * Close connection to weather server.
 *
 * @param weather_fd Socket for weather server
 */
void close_weather_connection(int weather_fd) {
//sending the close message to the server
send(weather_fd, "CLOSE\n", strlen("CLOSE\n"), 0);

//recieving the BYE message
char weather_close_response[BUFF_SIZE]; 
	int weather_close_bytes = recv(weather_fd, weather_close_response, BUFF_SIZE - 1, 0); //listen for the BYE\n or not message from the server
	if (weather_close_bytes > 0) {
		weather_close_response[weather_close_bytes] = '\0'; // force a null terminator at the end of what was received 
	}

//closing the server connection
close(weather_fd);	
}

/**
 * Talks to the server to get and display the requested weather information.
 * 
 * @pre Request type is not QUIT or INVALID
 * 
 * @param request The type of info being requested
 */
void handle_request(RequestType request) {
	// @todo: add code here that should happen for all types of requests
	char hostname[256];
	int port; 
	char password[256];

	if (get_server_info(hostname, &port, password) < 0) {
		return;
	}

	int weather_fd = authenticate_weather_server(hostname, port, password);
	if (weather_fd < 0) {
		return;
	}

	get_and_display_sensor_data(weather_fd, request);

	close_weather_connection(weather_fd);
}
