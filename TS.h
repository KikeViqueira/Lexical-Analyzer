#ifndef TS_H
#define TS_H
#include "abb.h"
// Función para inicializar la tabla y cargar las palabras reservadas función
void initTS();

// Función para buscar un lexema dentro de la tabla de símbolos, mete su numero correspondiente en la estructura
void buscarLexema(token *componente);

// Función para insertar un lexema en la tabla de símbolos
void insertarLexema(char *lexema_a_insertar,TIPOELEMENTOABB *nodo);

//Función para imprimir el contenido de la tabla de símbolos
void imprimirTablaSimbolos();

//Función para liberar la memoria usada por la tabla de símbolos
void liberarTS();

#endif