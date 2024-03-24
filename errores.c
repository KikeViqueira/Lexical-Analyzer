//
// Created by kike on 19/03/24.
//
#include <stdio.h>
#include "errores.h"

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

void reportarError(int estado){
    switch (estado) {
        case ERROR_FICHERO:
            printf(RED"\"Uso incorrecto del programa\\n Uso: <nombre ejecutable> <nombre archivo>\\n\""RESET);
            break;

        case ENTERO_INVALIDO:
            printf(BLUE"El entero que se esta procesando no es valido, pero sera enviado de todas maneras...\n"RESET);
            break;

        case FLOTANTE_INVALIDO:
            printf(BLUE"El flotante que se esta procesando no es valido, pero sera enviado de todas maneras...\n"RESET);
            break;

        case OP_DELI_INVALIDO:
            printf(BLUE"El operador/delimitador que se esta procesando no es valido, pero sera enviado de todas maneras...\n"RESET);
            break;

        case LITERAL_INVALIDO:
            printf(BLUE"El literal que se esta procesando no es valido, pero sera enviado de todas maneras...\n"RESET);
            break;

        case ID_INVALIDO:
            printf(BLUE"El identificador que se esta procesando no es valido, pero sera enviado de todas maneras...\n"RESET);
            break;

        case SUPERA_TAMANHO:
            printf(YELLOW"El lexema aceptado tiene un tamanho superior al permitido, nos quedaremos con lo maximo permitido desde el final hacia atras...\n"RESET);
            break;

        case CARACTER_NO_CONOCIDO:
            printf(RED"Caracter leido no reconocido por ningun automata\n"RESET);
            break;

        default:
            printf(RED"Se esta intentado notificar un error que no esta registrado\n"RESET);
            break;
    }
}

void lineaError(int c){
    printf(RED"El error ha tenido lugar en la linea %d del fichero fuente\n"RESET,c);
}