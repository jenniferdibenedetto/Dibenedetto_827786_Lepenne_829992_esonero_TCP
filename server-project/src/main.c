/*
 * main.c
 *
 * TCP Server - Template for Computer Networks assignment
 *
 * This file contains the boilerplate code for a TCP server
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
#include <time.h>
#include <ctype.h> 
#include "protocol.h"

#define NO_ERROR 0

void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}

//FUNZIONI DI SUPPORTO 

// Generatori di numeri casuali
float get_temperatura() { return ((float)rand() / (float)(RAND_MAX)) * 50.0 - 10.0; }
float get_umidita() { return ((float)rand() / (float)(RAND_MAX)) * 80.0 + 20.0; }
float get_vento() { return ((float)rand() / (float)(RAND_MAX)) * 100.0; }
float get_pressione() { return ((float)rand() / (float)(RAND_MAX)) * 100.0 + 950.0; }

//Controllo manuale della città 
int citta_valida(char *citta) {
	char *elenco_citta[] = {"bari", "roma", "milano", "napoli", "torino", 
                            "palermo", "genova", "bologna", "firenze", "venezia"};
	int i;
	for (i = 0; i < 10; i++) {
		if (strcasecmp(citta, elenco_citta[i]) == 0) return 1;
	}
	return 0;
}

int main(int argc, char *argv[]) {

	//VARIABILI
	int porta_server = SERVER_PORT;
	int socket_ascolto;
	int socket_client;
	struct sockaddr_in ind_server;
	struct sockaddr_in ind_client;
	int lungh_client;
	
	weather_request_t richiesta;
	weather_response_t risposta;
	int byte_ricevuti;
	int i;

	// Inizializzazione generatore casuale
	srand(time(NULL));

	// PARSING ARGOMENTI 
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-p") == 0) {
			i++;
			if (i < argc) porta_server = atoi(argv[i]);
		}
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

	int my_socket; 

	// Creazione socket
	my_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (my_socket < 0) {
		printf("Errore creazione socket.\n");
		clearwinsock();
		return -1;
	}

	// Configurazione indirizzo server
	memset(&ind_server, 0, sizeof(ind_server));
	ind_server.sin_family = AF_INET;
	ind_server.sin_port = htons(porta_server); 
	ind_server.sin_addr.s_addr = INADDR_ANY;   

	// Bind socket
	if (bind(my_socket, (struct sockaddr*)&ind_server, sizeof(ind_server)) < 0) {
		printf("Errore bind.\n");
		closesocket(my_socket);
		clearwinsock();
		return -1;
	}

	// Listen
	if (listen(my_socket, QUEUE_SIZE) < 0) {
		printf("Errore listen.\n");
		closesocket(my_socket);
		clearwinsock();
		return -1;
	}

	printf("Server in ascolto sulla porta %d\n", porta_server);

	// Loop di accettazione
	while (1) {
		lungh_client = sizeof(ind_client);
		
		// Accetta connessione
		socket_client = accept(my_socket, (struct sockaddr *)&ind_client, (socklen_t*)&lungh_client);
		
		if (socket_client < 0) {
			printf("Errore accept.\n");
			continue; 
		}

		// Ricezione Dati 
		byte_ricevuti = recv(socket_client, (char*)&richiesta, sizeof(richiesta), 0);

		if (byte_ricevuti > 0) {
			
			printf("Richiesta '%c %s' dal client ip %s\n", 
				   richiesta.type, 
				   richiesta.city, 
				   inet_ntoa(ind_client.sin_addr));

			// Prepara Risposta
			risposta.type = richiesta.type;
			risposta.status = 0; 

			// Validazione
			if (richiesta.type != 't' && richiesta.type != 'h' && richiesta.type != 'w' && richiesta.type != 'p') {
				risposta.status = 2; // Errore Richiesta
			} 
			else if (citta_valida(richiesta.city) == 0) {
				risposta.status = 1; // Errore Città
			}

			
			if (risposta.status == 0) {
				if (richiesta.type == 't') risposta.value = get_temperatura();
				if (richiesta.type == 'h') risposta.value = get_umidita();
				if (richiesta.type == 'w') risposta.value = get_vento();
				if (richiesta.type == 'p') risposta.value = get_pressione();
			} else {
				risposta.value = 0.0;
			}

			
			risposta.status = htonl(risposta.status);

			// Invio Risposta
			send(socket_client, (char*)&risposta, sizeof(risposta), 0);
		}

		
		closesocket(socket_client);
	}

	printf("Server terminated.\n");

	closesocket(my_socket);
	clearwinsock();
	return 0;
} 
