/*
 * protocol.h
 *
 * Client header file
 * Definitions, constants and function prototypes for the client
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H

// Shared Application Parameters
#define SERVER_PORT 56700 
#define BUFFER_SIZE 512
#define SERVER_IP "127.0.0.1" 

// Status Codes
#define STATO_SUCCESSO 0
#define STATO_CITTA_NON_DISPONIBILE 1
#define STATO_RICHIESTA_NON_VALIDA 2

// DEFINIZIONE STRUTTURE DATI 
// Struttura per la richiesta (Client -> Server)
typedef struct {
    char type;         // Tipo dati: 't', 'h', 'w', 'p'
    char city[64];     // Nome citta
} weather_request_t;

// Struttura per la risposta (Server -> Client)
typedef struct {
    unsigned int status; // 0=OK, 1=Err Citta, 2=Err Richiesta 
    char type;           // Eco del tipo
    float value;         // Valore meteo (temperatura, umidità, vento, pressione)
} weather_response_t;

//Function prototypes
float get_temperatura(void);
float get_umidita(void);
float get_vento(void);
float get_pressione(void);

#endif // PROTOCOL_H

