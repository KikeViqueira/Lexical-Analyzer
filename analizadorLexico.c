#include <stdio.h>
#include <stdlib.h>

#include "analizadorLexico.h"
#include "sistemaEntrada.h"

// Definimos la función de incialización del analizador léxico
void initLexico(char *archivo) {
    // llamamos al inicilizador del sistema de entrada pasandole el archivo por referencia
    initEntrada(archivo);
}

// Función que finaliza el analizador léxico y por tanto el sistema de entrada
void finalizarLexico() {
    finalizarEntrada();
}

// Función que devuelve el siguiente token del códgo fuente
char c = ' ';

token siguienteToken() {
    int estado = 0;
    while ((c = siguienteCaracter()) != EOF) {
        switch (estado) {
        case 0:
            if (c == ' ' || c == '\n' || c == '\t') {
                estado = 0;
            } else if (isapha(c) || c == '_') {
                // Si el caracter es una letra o un guión bajo, quien se encargará de procesar la cadena será el autómata de cadenas alfoanuméricas
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
}