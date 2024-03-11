#include "analizadorSintactico.h"
#include "analizadorLexico.h"
#include <stdio.h>

// Función que le da el control de ejecución al analizador sintáctico

void startAnalizadorSintactico() {

    //Creamos una variable de la estructura token, donde el analizador sintáctico guardará el componente léxico que le ha devuelto el analizador sintáctico
    token componente_lexico;

    //Iniciamos el analizador léxico pasándole el archivo por referencia
    //initLexico(archivo);

    //Iniciamos el bucle en el que el analizador sinbtactico pide componentes al léxico hasta que se procesa el archivo
    componente_lexico=siguienteToken();
    while (componente_lexico.numToken!=EOF){
        //Mientras sea distinto el sintáctico imprimirá por pantalla los componentes léxicos devueltos por el léxico
        printf("Componente lexico recibido -> <%d, %s>\n",componente_lexico.numToken,componente_lexico.lexema);
        componente_lexico=siguienteToken();
    }
}