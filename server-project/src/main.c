/*
 * main.c
 *
 * UDP Server - Template for Computer Networks assignment
 *
 * This file contains the boilerplate code for a UDP server
 * portable across Windows, Linux and macOS.
 */

#if defined WIN32
#include <winsock.h>
#else
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#define closesocket close
#endif

#include <stdio.h>
#include <stdlib.h>
#include "protocol.h"

#define NO_ERROR 0

void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}

int main(int argc, char *argv[]) {

	/*
	 * ====================================================================
	 * TODO 1: Parse command line arguments
	 * ====================================================================
	 * The server must accept the following optional argument:
	 *   -p <port> : Port number to listen on (default: 56700)
	 *
	 * Example: ./server-project -p 56700
	 *
	 * Hint: Use getopt() or manual parsing with strcmp()
	 */

#if defined WIN32
	// Initialize Winsock
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
	if (result != NO_ERROR) {
		printf("Error at WSAStartup()\n");
		return 0;
	}
#endif

	/*
	 * ====================================================================
	 * TODO 2: Create UDP socket
	 * ====================================================================
	 * Create a datagram socket using socket()
	 *
	 * Example:
	 *   int my_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	 *   if (my_socket < 0) {
	 *       printf("Socket creation failed\n");
	 *       return -1;
	 *   }
	 */
	int my_socket;

	/*
	 * ====================================================================
	 * TODO 3: Configure server address and bind socket
	 * ====================================================================
	 * 1. Set up the server address structure:
	 *    - Address family (AF_INET)
	 *    - Port number (from command line or default)
	 *    - IP address (INADDR_ANY to accept on all interfaces)
	 *
	 * 2. Bind the socket to the address
	 *
	 * Example:
	 *   struct sockaddr_in server_addr;
	 *   memset(&server_addr, 0, sizeof(server_addr));
	 *   server_addr.sin_family = AF_INET;
	 *   server_addr.sin_port = htons(port);
	 *   server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	 *
	 *   if (bind(my_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
	 *       printf("Bind failed\n");
	 *       return -1;
	 *   }
	 *
	 *   printf("Server listening on port %d\n", port);
	 */

	/*
	 * ====================================================================
	 * TODO 4: Implement weather data generation functions
	 * ====================================================================
	 * Implement the following functions to generate random weather data:
	 *
	 *   float get_temperature(void) - Returns value between -10.0 and 40.0 °C
	 *   float get_humidity(void)    - Returns value between 20.0 and 100.0 %
	 *   float get_wind(void)        - Returns value between 0.0 and 100.0 km/h
	 *   float get_pressure(void)    - Returns value between 950.0 and 1050.0 hPa
	 *
	 * Hint: Use rand() with proper scaling:
	 *   float random_range(float min, float max) {
	 *       return min + (float)rand() / RAND_MAX * (max - min);
	 *   }
	 *
	 * Don't forget to call srand(time(NULL)) once at the start
	 */

	/*
	 * ====================================================================
	 * TODO 5: Implement city validation
	 * ====================================================================
	 * The server must validate city names (case-insensitive).
	 * Supported cities: Bari, Roma, Milano, Napoli, Torino,
	 *                   Palermo, Genova, Bologna, Firenze, Venezia
	 *
	 * Hint: Create a function like:
	 *   int is_valid_city(const char* city) {
	 *       // Use strcasecmp() or stricmp() for case-insensitive comparison
	 *       // Return 1 if valid, 0 otherwise
	 *   }
	 */

	/*
	 * ====================================================================
	 * TODO 6: Implement datagram reception and processing loop
	 * ====================================================================
	 * The server runs indefinitely, processing requests as they arrive.
	 * For each request:
	 *   1. Receive datagram from client using recvfrom()
	 *   2. Convert request fields from network byte order
	 *   3. Log the request with client IP
	 *   4. Validate the request (check type and city)
	 *   5. Generate weather data if valid
	 *   6. Prepare response structure with appropriate status
	 *   7. Convert response fields to network byte order
	 *   8. Send response back to client using sendto()
	 *
	 * Example loop structure:
	 *
	 *   struct sockaddr_in client_addr;
	 *   int client_addr_len = sizeof(client_addr);
	 *
	 *   while (1) {
	 *       struct request req;
	 *       struct response resp;
	 *
	 *       // Receive request
	 *       int bytes_received = recvfrom(my_socket, &req, sizeof(req), 0,
	 *                                     (struct sockaddr*)&client_addr, &client_addr_len);
	 *
	 *       if (bytes_received < 0) {
	 *           printf("recvfrom() failed\n");
	 *           continue;
	 *       }
	 *
	 *       // Convert from network byte order
	 *       // ...
	 *
	 *       // Log request
	 *       char* client_ip = inet_ntoa(client_addr.sin_addr);
	 *       printf("Richiesta ricevuta da %s: type='%c', city='%s'\n",
	 *              client_ip, req.type, req.city);
	 *
	 *       // Validate and process request
	 *       if (!is_valid_city(req.city)) {
	 *           resp.status = 1;  // City not found
	 *       } else if (req.type != 't' && req.type != 'h' &&
	 *                  req.type != 'w' && req.type != 'p') {
	 *           resp.status = 2;  // Invalid request
	 *       } else {
	 *           resp.status = 0;  // Success
	 *           // Generate weather data based on type
	 *           switch (req.type) {
	 *               case 't': resp.value = get_temperature(); break;
	 *               case 'h': resp.value = get_humidity(); break;
	 *               case 'w': resp.value = get_wind(); break;
	 *               case 'p': resp.value = get_pressure(); break;
	 *           }
	 *       }
	 *
	 *       resp.type = req.type;  // Echo type
	 *
	 *       // Convert to network byte order
	 *       // ...
	 *
	 *       // Send response
	 *       int bytes_sent = sendto(my_socket, &resp, sizeof(resp), 0,
	 *                               (struct sockaddr*)&client_addr, client_addr_len);
	 *   }
	 */

	printf("Server terminated.\n");

	closesocket(my_socket);
	clearwinsock();
	return 0;
} // main end
