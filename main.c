#include "analizadorSintactico.h"
#include <stdlib.h>
#include "sistemaEntrada.h"
#include "errores.h"
#include "TS.h"

int main(int argc, char **argv) {
    if (argc != 2) {// Verificamos si el archivo se ha pasado por argumentos
        reportarError(ERROR_FICHERO);
        exit(EXIT_FAILURE);
    }
    initTS();// Inicializamos la tabla de símbolos
    initEntrada(argv[1]);//llamamos al sistema de entrada
    startAnalizadorSintactico();// Llamamos al analizador sintáctico
    imprimirTablaSimbolos();//Imprimir otra vez tabla de simbolos, para ver como ha cambiado
    liberarTS();//Liberamos la memoria usada por la tabla de simbolos
}
