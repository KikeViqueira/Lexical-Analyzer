#ifndef SISTEMAENTRADA_H
#define SISTEMAENTRADA_H

#include "analizadorLexico.h"


//Tamaño de cada buffer del sistema de entrada
#define TAM_BLOQUE 10

//estructura doble centinela
typedef struct {
    char bufferA[TAM_BLOQUE];
    char bufferB[TAM_BLOQUE];
    char *inicio;//Puntero que marca el inicio del lexema
    char *delantero;//Puntero que marca el caracter actual del lexema
}SistemaDobleCentinela;


// Funcion para inicializar el sistema de entrada
void initEntrada(char *archivo);

// Funcion para obtener el siguiente caracter
char siguienteCaracter();

// Funcion para retroceder caracter
void retrocederCaracter();

//Función que metera el lexema en el token, esta función es llamada por el léxico
void aceptar(token *componente);

//Funcion para saltar un carcter y no analizarlo
void omitirCaracter();

// Funcion para finalizar el sistema de entrada
//void finalizarEntrada();

#endif
