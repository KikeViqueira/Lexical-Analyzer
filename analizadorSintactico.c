#include "analizadorSintactico.h"
#include "analizadorLexico.h"
#include <stdio.h>
#include <stdlib.h>

// Función que le da el control de ejecución al analizador sintáctico

void startAnalizadorSintactico(char *archivo) {

    //Creamos una variable de la estructura token, donde el analizador sintáctico guardará el componente léxico que le ha devuelto el analizador sintáctico
    token componente_lexico;

    //Iniciamos el analizador léxico pasándole el archivo por referencia
    initLexico(archivo);






}