//
// Created by kike on 19/03/24.
//
#include <stdio.h>
#include "errores.h"

void reportarError(int estado){
    switch (estado) {

        case ERROR_FICHERO:
            printf("Error al abrir el archivo...\n");
            break;

        case LEXEMA_INVALIDO:
            printf("El lexema que se esta procesando no es valido, pero sera enviado de todas maneras...\n");
            break;

        case SUPERA_TAMANHO:
            printf("El lexema aceptado tiene un tamanho superior al permitido, nos quedaremos con lo maximo permitido desde el final hacia atras...\n");
            break;

    }
}