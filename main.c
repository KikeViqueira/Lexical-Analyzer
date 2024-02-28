#include "analizadorSintactico.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    // Verificamos si el archivo se ha pasado por argumentos
    if (argc != 2) {
        printf("Uso incorrecto del programa\n Uso: %s <nombre archivo>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /*Tenemos que incializar primero la tabla de símbolos y después darle el control de ejecución al analizador sintáctico,
    este una vez acabe de pedir los distintos tokens al analizador léxico e imprimirlos por pantalla, le devolverá el control al main.c
    para que termine la ejecución del programa y su correspondiente liberación de memoria.*/

    // Inicializamos la tabla de símbolos

    // Llamamos al analizador sintáctico
    startAnalizadorSintactico(argv[1]);
}