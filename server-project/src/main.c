/*
 * main.c
 *
 * UDP Server - Template for Computer Networks assignment
 *
 * This file contains the boilerplate code for a UDP server
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
#include <time.h>
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

void argumentPort_control(int argc, char* argv[], int* port){
	//se voglio cambiare il numero di porta da terminale
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
			*port = atoi(argv[++i]);
	    }
	}

	//controllo sul numero di porta
	if (*port <= 0 || *port > 65535) {
		fprintf(stderr, "Errore. Porta '%d' non valida.\n", *port);
		exit(EXIT_FAILURE);
    }
}

void errorhandler(char *errorMessage){
	printf("%s", errorMessage);
}


unsigned int checkResponse(weather_request_t *req) {

	if (req->type != 't' && req->type != 'h' && req->type != 'w' && req->type != 'p') {
	    return STATUS_INVALID_REQUEST; // Status 2
	}

	if (strpbrk(req->city, "@#$%&\t") != NULL) {
	    return STATUS_INVALID_REQUEST; // Status 2
	}


	for (int j = 0; j < CITY_NAME_SIZE && req->city[j]; j++) {
		req->city[j] = tolower((unsigned char)req->city[j]);
	}

	const char* valid_cities[] = {"bari", "roma", "milano", "napoli", "torino", "palermo", "genova", "bologna", "firenze", "venezia",NULL};

	unsigned int status = STATUS_CITY_NOT_FOUND; // Status 1

	for (int i = 0; valid_cities[i] != NULL; i++) {
		if (strcmp(req->city, valid_cities[i]) == 0) {
	        status = STATUS_SUCCESS; // Status 0
	            break;
        }
    }
    return status;
}


void deserializzaBuffer(uint8_t req_buffer[], weather_request_t *req){
	int offset = 0;
	//deserializza tipo
	req->type = req_buffer[offset];
	offset = offset + 1;
	//deserializza valore
	memcpy(req->city, &req_buffer[offset], CITY_NAME_SIZE);
	req->city[CITY_NAME_SIZE - 1] = '\0';
}

int serializzabuffer(uint8_t res_buffer[], weather_response_t res){
	int offset = 0;

	// Serializza status (con network byte order)
	uint32_t net_status = htonl(res.status);
	memcpy(res_buffer + offset, &net_status, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	// Serializza type (1 byte, no conversione)
	memcpy(res_buffer + offset, &res.type, sizeof(char));
	offset += sizeof(char);

	// Serializza value (float con network byte order)
	uint32_t temp;
	memcpy(&temp, &res.value, sizeof(float));
	temp = htonl(temp);
	memcpy(res_buffer + offset, &temp, sizeof(float));
	offset += sizeof(float);

	return offset;
}

float get_temperature(void){ // Range: -10.0 to 40.0 °C
	float random_base = (float)rand() / (float)RAND_MAX; //trova un vero float casuale
	float temperature = (random_base * 50.0) - 10.0;
	return temperature;
}
float get_humidity(void){ // Range: 20.0 to 100.0 %
	float random_base = (float)rand() / (float)RAND_MAX;
	float humidity = (random_base * 80.0) + 20.0;
	return humidity;
}
float get_wind(void){  // Range: 0.0 to 100.0 km/h
	float random_base = (float)rand() / (float)RAND_MAX;
	float wind = (random_base * 100.0);
	return wind;
}
float get_pressure(void){  // Range: 950.0 to 1050.0 hPa
	float random_base = (float)rand() / (float)RAND_MAX;
	float pressure = (random_base * 100.0) + 950.0;
	return pressure;
}

float set_value(char type){
	float value = 0.0;
	switch(type){
		case TEMPERATURE:{
			value = get_temperature();
			break;
		}
		case HUMIDITY:{
			value = get_humidity();
			break;
		}
		case WIND_SPEED:{
			value = get_wind();
			break;
		}
		case PRESSURE:{
			value = get_pressure();
			break;
		}
	}
	return value;
}


int main(int argc, char *argv[]) {
	//CONTROLLA IL NUMERO DI PORTA PASSATO TRAMITE ARGOMENTO
	int port = SERVER_PORT;
	argumentPort_control(argc, argv, &port);


#if defined WIN32
	// Initialize Winsock
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
	if (result != NO_ERROR) {
		printf("Error at WSAStartup()\n");
		return 0;
	}
#endif

	srand(time(NULL)); //PERMETTE LA GENERAZIONE DI NUMERI PSEUDO-CASUALI PER I VALORI METEO
	int my_socket;
	my_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(my_socket < 0){
		errorhandler("Creazione della socket fallita.\n");
		clearwinsock();
		return -1;
	}

	// CONFIGURAZIONE DELLA SOCKET
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = INADDR_ANY;


	//ASSEGNAZIONE DELLA SOCKET
	if(bind(my_socket, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0){
		errorhandler("bind() fallito.\n");
		closesocket(my_socket);
		clearwinsock();
		return -1;
	}

	//SERVER IN ASCOLTO
	printf("Server in ascolto sulla porta %d...\n", port);
	while (1) {

		struct sockaddr_in cad; //struttura per l'indirizzo del client
		int client_len = sizeof(cad);

		//RICEZIONE DELLA RICHIESTA DEL CLIENT
		weather_request_t req;
		int bytes_rcvd;
		uint8_t req_buffer[REQ_BUFFER_SIZE];
		if((bytes_rcvd = recvfrom(my_socket, (char*)&req_buffer, REQ_BUFFER_SIZE, 0, (struct sockaddr*)&cad, &client_len)) <= 0){
			errorhandler("recv(req) fallito o connessione chiusa prematuramente.\n");
			closesocket(my_socket);
			clearwinsock();
			return -1;
		}

		//DESERIALIZZAZIONE DATI
		deserializzaBuffer(req_buffer, &req);

		//RISOLUZIONE DELL'INDIRIZZO
		struct hostent *remoteHost;
		remoteHost = gethostbyaddr((char*)&cad.sin_addr, 4, AF_INET);
		char *host_name = (remoteHost) ? remoteHost->h_name : "Unknown";
		printf("Richiesta ricevuta da %s (ip %s): type='%c', city='%s'\n", host_name, inet_ntoa(cad.sin_addr), req.type, req.city);

		//VALIDAZIONE RICHIESTA
		weather_response_t res;
		res.status = checkResponse(&req);

		//COSTRUZIONE RISPOSTA
		switch(res.status){
			case STATUS_SUCCESS:{
				float value = set_value(req.type);
				res.type = req.type;
				res.value = value;
				break;
			}
			case STATUS_CITY_NOT_FOUND:{
				res.type = req.type;
				res.value = 0.0;
				break;
			}
			case STATUS_INVALID_REQUEST:{
				res.type = req.type;
				res.value = 0.0;
				break;
			}
			default:
				printf("Status errato\n");
		}

		//SERIALIZZAZIONE DATI
		uint8_t res_buffer[RES_BUFFER_SIZE];
		int res_offset = serializzabuffer(res_buffer, res);
		//INVIO RISPOSTA
		if(sendto(my_socket,(char*)&res_buffer, res_offset, 0, (struct sockaddr*)&cad, sizeof(cad)) != res_offset){
			errorhandler("send(res) ha inviato un numero di bytes diverso da quanti ne erano attesi.\n");
			return -1;
		}
	}

	printf("Server terminato.\n");
	//CHIUSURA SOKET
	closesocket(my_socket);
	clearwinsock();
	return 0;
} // main end


