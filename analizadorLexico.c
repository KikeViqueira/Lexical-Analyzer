#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "analizadorLexico.h"
#include <ctype.h>
#include "sistemaEntrada.h"
#include "TS.h"
#include "definiciones.h"

// Funciones
void automataAlphaNumerico( char c,token *componente);

// Definimos la función de incialización del analizador léxico
void initLexico(char *archivo) {
    // llamamos al inicilizador del sistema de entrada pasándole el archivo por referencia
    initEntrada(archivo);
}

// Función que finaliza el analizador léxico y por tanto el sistema de entrada
void finalizarLexico() {
    //finalizarEntrada();
}

// Autómata alphanumérico
token siguienteToken() {
    token componente;
    componente.lexema = NULL;

    char c;
    int estado = 0;
    while ((c = siguienteCaracter()) != EOF) {
        switch (estado) {
        case 0:
            if (c == ' ' || c == '\n' || c == '\t') {
                estado = 0;
            } else if (isalpha(c) || c == '_') {
                // Si el caracter es una letra o un guión bajo, quien se encargará de procesar la cadena será el autómata de cadenas alfoanuméricas
                automataAlphaNumerico(c,&componente);

            } else if (isdigit(c)) {
                // Si el caracter es un dígito, quien se encargará de procesar la cadena será el autómata de números
                estado = 0;
            }

            break;

        default:
            printf("No se ha podido reconocer el caracter por ningun automata\n");
            break;
        }
    }
    //buscamos en la tabla de simbolos si existe el lexema para saber su numero de token

    //Devolvemos el componente al analizador sintactico
    return componente;
}

void automataAlphaNumerico(char c, token *componente) {
    int capacidad = 7; //Tamaño inicial del buffer
    char *bufferAux = (char*)malloc(sizeof (char)*capacidad);
    int indice = 0;
    //El primer caracter de la cadena ya es procesado por el switch q elige el autómata correspondiente
    bufferAux[indice++]=c; //Primero asigna y ps incrementa el indice

    while ((c=siguienteCaracter())!=EOF && (isalnum(c) || c=='_')){
        if (indice + 1 >= capacidad) { // +1 para el carácter nulo
            capacidad *= 2; // Duplica la capacidad
            char *aumento = realloc(bufferAux, capacidad * sizeof(char));
            if (aumento == NULL) {
                printf("Error al hacer realloc\n");
                free(bufferAux);
                exit(EXIT_FAILURE);
            }
            bufferAux = aumento;
        }
        bufferAux[indice++] = c;
    }

    if (c != EOF) {
        retrocederCaracter(); // El último carácter leído no forma parte del token
    }

    bufferAux[indice] = '\0'; // Termina la cadena
    componente->lexema = strdup(bufferAux); // Copia el buffer auxiliar al lexema del token
    free(bufferAux); // Libera el buffer auxiliar
}