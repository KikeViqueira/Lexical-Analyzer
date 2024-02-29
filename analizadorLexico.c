#include <stdio.h>
#include <stdlib.h>
#include "analizadorLexico.h"
#include <ctype.h>
#include "sistemaEntrada.h"

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
                estado = 0;
            } else if (isdigit(c)) {
                // Si el caracter es un dígito, quien se encargará de procesar la cadena será el autómata de números
                estado = 0;
            }

            break;

        default:
            break;
        }
    }

    return componente;
}

void automataAlphaNumerico(char c, token *componente) {
    int estado=0;
    switch (estado) {
        case 0:
            if (isalnum(c)||c=='_') {
                estado=0;

                c=siguienteCaracter();
            }
            else {
                estado=1;
            }
            break;
        case 1:
            if (c !=' ')
                retrocederCaracter();
            break;
    }


}