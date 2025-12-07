/*
 * main.c
 *
 * UDP Client - Template for Computer Networks assignment
 *
 * This file contains the boilerplate code for a UDP client
 * portable across Windows, Linux, and macOS.
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
#include <string.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include "protocol.h"

#define NO_ERROR 0

void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}

void errorhandler(char *errorMessage){
	printf("%s", errorMessage);
}

void arguments_error(const char* prog_name) {
    fprintf(stderr, "Usa: %s [-s server] [-p port] -r \"type city\"\n", prog_name);
    fprintf(stderr, "  -s server: IP address/server hostname(default: %s)\n", DEFAULT_HOST);
    fprintf(stderr, "  -p port:   Server port (default: %d)\n", SERVER_PORT);
    fprintf(stderr, "  -r request: Richiesta obbligatoria (eg. \"t bari\")\n");
    exit(EXIT_FAILURE);
}

void control_arguments(int argc, char* argv[], char** server_host, int* port, char** request_str){
	if (argc < 3) {
			arguments_error(argv[0]);
		}

	    for (int i = 1; i < argc; i++) {
	        if (strcmp(argv[i], "-s") == 0) {
	            if (++i < argc) *server_host = argv[i];
	            else arguments_error(argv[0]);
	        } else if (strcmp(argv[i], "-p") == 0) {
	            if (++i < argc) *port = atoi(argv[i]);
	            else arguments_error(argv[0]);
	        } else if (strcmp(argv[i], "-r") == 0) {
	            if (++i < argc) *request_str = argv[i];
	            else arguments_error(argv[0]);
	        } else {
	            fprintf(stderr, "Errore. Opzione sconosciuta: '%s'\n", argv[i]);
	            arguments_error(argv[0]);
	        }
	    }

	    // CONTROLLA CHE CI SIA -r
	    if (*request_str == NULL) {
	        fprintf(stderr, "Errore: il parametro -r è obbligatorio.\n");
	        arguments_error(argv[0]);
	    }


	    // CONTROLLA CHE LA PORTA SIA VALIDA
	    if (*port <= 0 || *port > 65535) {
	        fprintf(stderr, "Errore. Numero di porta: '%d' non valido.\n", *port);
	        exit(EXIT_FAILURE);
	    }
}

int serializzaBuffer(uint8_t buffer[], weather_request_t req){
	size_t offset = 0;
	// Serializza type (1 byte, no conversione)
	buffer[offset] = (uint8_t)req.type;
	offset += sizeof(uint8_t);

	// Serializza city
	memcpy(buffer + offset, req.city, CITY_NAME_SIZE);
	offset += CITY_NAME_SIZE;

	return offset;
}

void deserializzaBuffer(uint8_t res_buffer[], weather_response_t *res){
	int offset = 0;
	//deserializza status
	uint32_t net_status;
	memcpy(&net_status, &res_buffer[offset], sizeof(net_status));
	res->status = (unsigned int)ntohl(net_status);
	offset = offset + sizeof(net_status);
	//deserializza tipo
	res->type = res_buffer[offset];
	offset = offset + 1;
	//deserializza valore
	uint32_t net_value;
	memcpy(&net_value, &res_buffer[offset], sizeof(net_value));
	uint32_t host_value = ntohl(net_value);
	memcpy(&res->value, &host_value, sizeof(res->value));
	offset = offset + sizeof(net_value);
}

void capitalize_city(char *str) {
    if (str && *str) {
        *str = toupper((unsigned char)*str);
        for (int i = 1; str[i]; i++){
        	str[i] = tolower((unsigned char)str[i]);
        }
    }
}

void build_msg(weather_response_t res, weather_request_t req){
	char displayCity[CITY_NAME_SIZE];
	strncpy(displayCity, req.city, CITY_NAME_SIZE-1);
	displayCity[CITY_NAME_SIZE - 1] = '\0';
	capitalize_city(displayCity);

	switch(res.type){
		case TEMPERATURE:{
			printf("%s: Temperatura = %.1f°C\n", displayCity, res.value);
			break;
		}
		case HUMIDITY:{
			printf("%s: Umidita' = %.1f%%\n", displayCity, res.value);
			break;
		}
		case WIND_SPEED:{
			printf("%s: Vento = %.1f km/h\n", displayCity, res.value);
			break;
		}
		case PRESSURE:{
			printf("%s: Pressione = %.1f hPa\n", displayCity, res.value);
			break;
		}
	}
}

int main(int argc, char *argv[]) {


	char* server_host = DEFAULT_HOST;
	int port = SERVER_PORT;
	char* request_str = NULL;
	// CONTROLLO ARGOMENTI DA RIGA DI COMANDO
	control_arguments(argc, argv, &server_host, &port, &request_str);
	if (strchr(request_str, '\t') != NULL) {
		fprintf(stderr, "Errore: La richiesta non puo' contenere tabulazioni.\n");
	    return -1;
	}
	if (strlen(request_str) < 3) {
		arguments_error(argv[0]);
	}

	//INIZIALIZZAZIONE WINSOCK
	#if defined WIN32
	    WSADATA wsa_data;
	    int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
	    if (result != NO_ERROR) {
	    	printf("Error at WSAStartup()\n");
	    	return 0;
	    }
	#endif

	//RISOLUZIONE DEI NOMI DEL DNS
	struct hostent *host;
	struct in_addr addr;
	if(isalpha(server_host[0])){
		host = gethostbyname(server_host);
	}
	else{
		addr.s_addr = inet_addr(server_host);
		host = gethostbyaddr((char*)&addr,4,AF_INET);
	}
	if (host == NULL) {
	    fprintf(stderr, "Errore risoluzione host: %s\n", server_host);
	    clearwinsock();
	    return -1;
	}

	char *city_ptr = request_str + 1;
	// Salta spazi tra il tipo e il nome della città
	while (*city_ptr && isspace((unsigned char)*city_ptr)) {
		city_ptr++;
	}

	if (*city_ptr == '\0') {
		fprintf(stderr, "Errore: nome città mancante.\n");
		clearwinsock();
	    exit(EXIT_FAILURE);
	}
    //CONTROLLA CHE IL TIPO SIA FORMATO DA UN SINGOLO CARATTERE
	if(request_str[1] != ' '){ //controlla che dopo il primo carattere (tipo) ci sia uno spazio, quindi il tipo è espresso come singola lettera
		fprintf(stderr, "Errore: il tipo della richiesta deve essere formato da un singolo carattere (es. t/h/p/w).\n");
		exit(EXIT_FAILURE);
	}
	if(strlen(city_ptr) >= CITY_NAME_SIZE){
		fprintf(stderr, "Errore: nome citta' troppo lungo (max %d char).\n", CITY_NAME_SIZE - 1);
	    clearwinsock();
        exit(EXIT_FAILURE);
    }

    // COSTRUZIONE RICHIESTA CLIENT
    weather_request_t req;
    memset(&req, 0, sizeof(req));
    req.type = request_str[0];
    strncpy(req.city, city_ptr, CITY_NAME_SIZE - 1); // Copia il resto della stringa (dopo "t ") nella città
    req.city[CITY_NAME_SIZE - 1] = '\0';  // Assicura la terminazione

	//CREAZIONE DELLA SOCKET
	int my_socket;
	my_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(my_socket < 0){
		errorhandler("Creazione della socket fallita.\n");
		clearwinsock();
	return -1;
	}

	//COSTRUZIONE DELL'INDIRIZZO DEL SERVER
	struct sockaddr_in sad;
	memset(&sad, 0, sizeof(sad));
	sad.sin_family = AF_INET;
	memcpy(&sad.sin_addr, host->h_addr, host->h_length);
	sad.sin_port = htons(port);

	//INVIO RICHIESTA METEO
	uint8_t req_buffer[sizeof(uint8_t) + CITY_NAME_SIZE];
	int req_offset = serializzaBuffer(req_buffer, req);
	if(sendto(my_socket, (const char*)req_buffer, req_offset, 0, (struct sockaddr*)&sad, sizeof(sad)) != req_offset){
		errorhandler("sendto() ha mandato un numero di bytes differente da quanti ne erano previsti\n");
	}

	//RICEZIONE RISPOSTA SERVER
	weather_response_t res;
	uint8_t res_buffer[sizeof(uint32_t) + sizeof(char) + sizeof(float)];
	struct sockaddr_in fromAddr;
	#if defined WIN32
        int fromsize = sizeof(fromAddr);
    #else
        socklen_t fromsize = sizeof(fromAddr);
    #endif

    int respStringLen;
    respStringLen = recvfrom(my_socket, (char *)res_buffer, sizeof(res_buffer), 0, (struct sockaddr*)&fromAddr, &fromsize);
    if(respStringLen < 0){
        errorhandler("recvfrom() failed.");
        return -1;
    }

    if(sad.sin_addr.s_addr != fromAddr.sin_addr.s_addr){
        errorhandler("Error: received packet from unknown source.");
    }
    deserializzaBuffer(res_buffer, &res);

	//COSTRUZIONE DEL MESSAGGIO
	printf("Ricevuto risultato dal server %s (ip %s). ", host->h_name, inet_ntoa(fromAddr.sin_addr));
	switch(res.status){
		case STATUS_SUCCESS:{
			build_msg(res, req);
			break;
		}
		case STATUS_CITY_NOT_FOUND:{
			printf("Citta' non disponibile\n");
			break;
		}
		case STATUS_INVALID_REQUEST:{
			printf("Richiesta non valida\n");
			break;
		}
		default: printf("Stato sconosciuto.\n");
	}


	//CHIUSURA CONNESSIONE
	printf("Terminazione del client.\n");
	closesocket(my_socket);
	clearwinsock();
	return 0;
} // main end
