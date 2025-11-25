/*
 * protocol.h
 *
 * Server header file
 * Definitions, constants and function prototypes for the server
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

// Shared application parameters
#define SERVER_PORT 56700
#define BUFFER_SIZE 512
#define QUEUE_SIZE 6     

//DEFINIZIONE STRUTTURE DATI 
// Struttura per la richiesta (Client -> Server)
typedef struct {
    char type;        // Tipo dati: 't', 'h', 'w', 'p'
    char city[64];    // Nome citta
} weather_request_t;

// Struttura per la risposta (Server -> Client)
typedef struct {
    unsigned int status;  // 0=OK, 1=Err Citta, 2=Err Richiesta
    char type;            // Eco del tipo
    float value;          // Valore meteo
} weather_response_t;

#endif /* PROTOCOL_H_ */
