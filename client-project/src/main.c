/*
 * main.c
 *
 * UDP Client - Template for Computer Networks assignment
 *
 * This file contains the boilerplate code for a UDP client
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
	 * The client must accept the following arguments:
	 *   -s <server>  : Server hostname or IP address (default: "localhost")
	 *   -p <port>    : Server port number (default: 56700)
	 *   -r <request> : Weather request in format "type city"
	 *                  where type is 't', 'h', 'w', or 'p'
	 *
	 * Example: ./client-project -s localhost -p 56700 -r "t Roma"
	 *
	 * Hint: Use getopt() or manual parsing with strcmp()
	 * Validate that -r argument is provided (it's mandatory)
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
	 * TODO 3: Configure server address
	 * ====================================================================
	 * Set up the server address structure with:
	 *   - Address family (AF_INET)
	 *   - Port number (from command line or default)
	 *   - IP address (use getaddrinfo() for hostname resolution)
	 *
	 * Example:
	 *   struct sockaddr_in server_addr;
	 *   memset(&server_addr, 0, sizeof(server_addr));
	 *   server_addr.sin_family = AF_INET;
	 *   server_addr.sin_port = htons(port);
	 *
	 * For hostname resolution, use getaddrinfo() instead of inet_addr()
	 * to support both hostnames (e.g., "localhost") and IP addresses
	 */

	/*
	 * ====================================================================
	 * TODO 4: Prepare and send weather request
	 * ====================================================================
	 * 1. Parse the request string (e.g., "t Roma") to extract type and city
	 * 2. Create a struct request and populate it
	 * 3. Convert struct fields to network byte order (htonl, htons)
	 * 4. Send the request using sendto()
	 *
	 * Example:
	 *   struct request req;
	 *   req.type = 't';  // parsed from request string
	 *   strncpy(req.city, "Roma", MAX_CITY_LENGTH);
	 *
	 *   // Convert to network byte order before sending
	 *   // (handle multi-byte fields appropriately)
	 *
	 *   int bytes_sent = sendto(my_socket, &req, sizeof(req), 0,
	 *                           (struct sockaddr*)&server_addr, sizeof(server_addr));
	 */

	/*
	 * ====================================================================
	 * TODO 5: Receive and process server response
	 * ====================================================================
	 * 1. Receive the response using recvfrom()
	 * 2. Convert struct fields from network byte order (ntohl, ntohs)
	 * 3. Check the status field and display appropriate message
	 *
	 * Output format:
	 *   "Ricevuto risultato dal server [IP]. [message]"
	 *
	 * Where [message] depends on status:
	 *   - status 0: "CityName: Type = value unit"
	 *   - status 1: "Città non disponibile"
	 *   - status 2: "Richiesta non valida"
	 *
	 * Example:
	 *   struct response resp;
	 *   struct sockaddr_in from_addr;
	 *   int from_len = sizeof(from_addr);
	 *
	 *   int bytes_received = recvfrom(my_socket, &resp, sizeof(resp), 0,
	 *                                 (struct sockaddr*)&from_addr, &from_len);
	 *
	 *   // Convert from network byte order
	 *   // Print formatted output based on resp.status
	 */

	/*
	 * ====================================================================
	 * TODO 6: Cleanup and close socket
	 * ====================================================================
	 */
	// closesocket(my_socket);

	printf("Client terminated.\n");

	clearwinsock();
	return 0;
} // main end
