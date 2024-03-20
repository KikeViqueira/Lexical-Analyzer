#include "analizadorSintactico.h"
#include "analizadorLexico.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>

// Función que le da el control de ejecución al analizador sintáctico

void startAnalizadorSintactico() {

    //Creamos una variable de la estructura token, donde el analizador sintáctico guardará el componente léxico que le ha devuelto el analizador léxico
    token* componente_lexico;

    //Iniciamos el bucle en el que el analizador sinbtactico pide componentes al léxico hasta que se procesa el archivo
    componente_lexico=siguienteToken();
    while (componente_lexico->numToken!=EOF){
        //Mientras sea distinto el sintáctico imprimirá por pantalla los componentes léxicos devueltos por el léxico
        if(strcmp(componente_lexico->lexema,"\n")==0) {
            //Printf para que los \n se impriman de manera literal en la terminal
            printf("Componente lexico recibido -> <%d, \\n>\n",componente_lexico->numToken);
        }
        else{
            printf("Componente lexico recibido -> <%d, %s>\n",componente_lexico->numToken,componente_lexico->lexema);
        }
        //una vez que se hemos imprimido el token por pantalla liberamos su memoria para el siguiente token que se va a procesar
        free(componente_lexico->lexema);
        free(componente_lexico);
        componente_lexico=siguienteToken();
    }
    free(componente_lexico->lexema);
    free(componente_lexico);
    componente_lexico=NULL;

}