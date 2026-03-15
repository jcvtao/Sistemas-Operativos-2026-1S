#ifndef STRUCTURES_H
#define STRUCTURES_H

#define MAX_STR 90
#define HASH_SIZE 500000 

typedef struct {
    char name[MAX_STR];
    char domain[MAX_STR];
    char year[10];
    char industry[MAX_STR];
    char locality[MAX_STR];
    char country[MAX_STR];
    char linkedin[120];
    long next_name;     // Puntero para colisiones de nombre
    long next_criteria; // Puntero para colisiones de pais+industria
} Company;

// Para la comunicación entre procesos
typedef struct {
    int type; // 1: Nombre, 2: Pais+Industria, 3: Salir
    char key1[MAX_STR]; // Nombre o Pais
    char key2[MAX_STR]; // Industria
} SearchRequest;

#endif