/*
 * protocol.h
 *
 * Shared header file for UDP client and server
 * Contains protocol definitions, data structures, constants and function prototypes
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <stdint.h>

/*
 * ============================================================================
 * PROTOCOL CONSTANTS
 * ============================================================================
 */

// Server port - Default port for the weather service
#define SERVER_PORT 56700

// Buffer size for network messages
#define BUFFER_SIZE 512

// Maximum length for city name (including null terminator)
#define MAX_CITY_LENGTH 64

/*
 * ============================================================================
 * PROTOCOL DATA STRUCTURES
 * ============================================================================
 */

/*
 * Weather request structure
 * Sent by client to request weather data for a specific city
 *
 * Fields:
 *   - type: Weather type ('t'=temperature, 'h'=humidity, 'w'=wind, 'p'=pressure)
 *   - city: City name (null-terminated string, max 64 chars including '\0')
 *
 * Note: City name may contain multiple spaces but no tab characters ('\t')
 */
struct request {
    char type;              // Weather data type
    char city[MAX_CITY_LENGTH];  // City name (null-terminated)
};

/*
 * Weather response structure
 * Sent by server in response to a client request
 *
 * Fields:
 *   - status: Response status code
 *             0 = success
 *             1 = city not found
 *             2 = invalid request
 *   - type: Echo of the requested weather type
 *   - value: Weather data value (meaningful only if status == 0)
 */
struct response {
    unsigned int status;    // Status code
    char type;              // Echo of requested type
    float value;            // Weather data value
};

/*
 * ============================================================================
 * FUNCTION PROTOTYPES
 * ============================================================================
 */

// Add here the signatures of the functions you implement

/*
 * Example functions that might be useful:
 *
 * // Send a weather request to the server
 * int send_request(int socket, struct request* req, struct sockaddr_in* server_addr);
 *
 * // Receive a weather response from the server
 * int receive_response(int socket, struct response* resp, struct sockaddr_in* from_addr);
 *
 * // Parse command line arguments
 * int parse_arguments(int argc, char* argv[], char** server, int* port, char** request_str);
 */

#endif /* PROTOCOL_H_ */
