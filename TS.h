#ifndef TS_H
#define TS_H
#include "abb.h"
// Función para inicializar la tabla y cargar las palabras reservadas función
void initTS();

// Función para buscar un lexema dentro de la tabla de símbolos, devuelve su numero correspondiente
int buscarLexema(char *lexema_a_buscar);

// Función para insertar un lexema en la tabla de símbolos
void insertarLexema(char *lexema_a_insertar,TIPOELEMENTOABB *nodo);

#endif