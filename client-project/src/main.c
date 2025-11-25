/*
 * main.c
 *
 * TCP Client - Template for Computer Networks assignment
 *
 * This file contains the boilerplate code for a TCP client
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

	//VARIABILI 
	int porta_server = SERVER_PORT;
	char *ip_server = "127.0.0.1"; 
	char *stringa_input = NULL;
	
	int socket_client;
	struct sockaddr_in indirizzo_server;
	
	weather_request_t richiesta;
	weather_response_t risposta;
	
	int i; 
	int byte_inviati;
	int byte_ricevuti;

	//PARSING ARGOMENTI 
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-s") == 0) {
			i++;
			if (i < argc) ip_server = argv[i];
		}
		else if (strcmp(argv[i], "-p") == 0) {
			i++;
			if (i < argc) porta_server = atoi(argv[i]);
		}
		else if (strcmp(argv[i], "-r") == 0) {
			i++;
			if (i < argc) stringa_input = argv[i];
		}
	}

	// Controllo obbligatorio: la richiesta (-r) deve esserci
	if (stringa_input == NULL) {
		printf("Errore. Uso corretto: %s -r \"tipo citta\" [-s server] [-p porta]\n", argv[0]);
		return 0;
	}

	//PREPARAZIONE MESSAGGIO 
	memset(&richiesta, 0, sizeof(richiesta));
	
	// Il primo carattere è il tipo
	richiesta.type = stringa_input[0];
	
	// Copiamo il nome della città (saltando i primi 2 caratteri se c'è lo spazio, es "t bari")
	if (strlen(stringa_input) > 2) {
		strncpy(richiesta.city, &stringa_input[2], 63);
	} else {
		strncpy(richiesta.city, &stringa_input[1], 63);
	}

#if defined WIN32
	// Initialize Winsock
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
	if (result != NO_ERROR) {
		printf("Error at WSAStartup()\n");
		return 0;
	}
#endif

	// Creazione socket
	socket_client = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socket_client < 0) {
		printf("Errore creazione socket.\n");
		clearwinsock();
		return -1;
	}

	// Configurazione indirizzo server
	memset(&indirizzo_server, 0, sizeof(indirizzo_server));
	indirizzo_server.sin_family = AF_INET;
	indirizzo_server.sin_port = htons(porta_server); // Conversione Host to Network Short
	indirizzo_server.sin_addr.s_addr = inet_addr(ip_server);

	// Connessione al server
	if (connect(socket_client, (struct sockaddr *)&indirizzo_server, sizeof(indirizzo_server)) < 0) {
		printf("Errore connessione al server.\n");
		closesocket(socket_client);
		clearwinsock();
		return -1;
	}

	//INVIO 
	byte_inviati = send(socket_client, (char*)&richiesta, sizeof(richiesta), 0);
	if (byte_inviati != sizeof(richiesta)) {
		printf("Errore invio dati.\n");
		closesocket(socket_client);
		clearwinsock();
		return -1;
	}

	//RICEZIONE 
	byte_ricevuti = recv(socket_client, (char*)&risposta, sizeof(risposta), 0);
	if (byte_ricevuti <= 0) {
		printf("Errore ricezione dati o connessione chiusa.\n");
		closesocket(socket_client);
		clearwinsock();
		return -1;
	}

	//GESTIONE ENDIANNESS 
	risposta.status = ntohl(risposta.status);

	//STAMPA RISULTATO
	printf("Ricevuto risultato dal server ip %s. ", ip_server);

	if (risposta.status == 0) {
		// Maiuscola estetica
		if (richiesta.city[0] >= 'a' && richiesta.city[0] <= 'z') {
			richiesta.city[0] = richiesta.city[0] - 32;
		}

		if (risposta.type == 't') {
			printf("%s: Temperatura = %.1f°C\n", richiesta.city, risposta.value);
		}
		else if (risposta.type == 'h') {
			printf("%s: Umidità = %.1f%%\n", richiesta.city, risposta.value);
		}
		else if (risposta.type == 'w') {
			printf("%s: Vento = %.1f km/h\n", richiesta.city, risposta.value);
		}
		else if (risposta.type == 'p') {
			printf("%s: Pressione = %.1f hPa\n", richiesta.city, risposta.value);
		}
		else {
			printf("Tipo risposta sconosciuto\n");
		}
	} 
	else if (risposta.status == 1) {
		printf("Città non disponibile\n");
	} 
	else if (risposta.status == 2) {
		printf("Richiesta non valida\n");
	}

	closesocket(socket_client);
	clearwinsock();
	return 0;
} // main end
