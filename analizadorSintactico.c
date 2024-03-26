#include "analizadorSintactico.h"
#include "analizadorLexico.h"
#include <stdio.h>
#include <string.h>

#define RESET   "\x1b[0m"
#define RED     "\x1b[31m"
#define MAGENTA "\x1b[35m"
#define BLUE    "\x1b[34m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define CYAN    "\x1b[36m"

// Función que le da el control de ejecución al analizador sintáctico
void startAnalizadorSintactico() {
    //Creamos una variable de la estructura token, donde el analizador sintáctico guardará el componente léxico que le ha devuelto el analizador léxico
    token componente_lexico;

    //Iniciamos el bucle en el que el analizador sinbtactico pide componentes al léxico hasta que se procesa el archivo
    componente_lexico=siguienteToken();
    while (componente_lexico.numToken!=EOF){
        //Mientras sea distinto el sintáctico imprimirá por pantalla los componentes léxicos devueltos por el léxico
        if(strcmp(componente_lexico.lexema,"\n")==0) {
            //Printf para que los \n se impriman de manera literal en la terminal
            printf("< %d, \\n >\n",componente_lexico.numToken);
        }
        else{
            if(componente_lexico.numToken>=273 && componente_lexico.numToken<=308) printf("< "RED"%d, %s >\n"RESET,componente_lexico.numToken,componente_lexico.lexema);//Palabras reservadas
            else if(componente_lexico.numToken==309) printf("< "MAGENTA"%d, %s >\n"RESET,componente_lexico.numToken,componente_lexico.lexema);//Identificadores
            else if(componente_lexico.numToken==310) printf("< "BLUE"%d, %s >\n"RESET,componente_lexico.numToken,componente_lexico.lexema);//Enteros
            else if(componente_lexico.numToken==311) printf("< "GREEN"%d, %s >\n"RESET,componente_lexico.numToken,componente_lexico.lexema);//Flotantes
            else if(componente_lexico.numToken>=313 && componente_lexico.numToken<=335) printf("< "YELLOW"%d, %s >\n"RESET,componente_lexico.numToken,componente_lexico.lexema);//Operadores-Delimitadores compuestos
            else if(componente_lexico.numToken==337) printf("< "CYAN"%d, %s >\n"RESET,componente_lexico.numToken,componente_lexico.lexema);//Literales
            else printf("< %d, %s >\n",componente_lexico.numToken,componente_lexico.lexema);//El resto
        }
        //Una vez ha llegado un token pedimos el siguiente y así sucesivamente
        componente_lexico=siguienteToken();
    }
    //Imprimimos por pantalla el componente del EOF
    printf("< %d, %s >\n",componente_lexico.numToken,componente_lexico.lexema);

    //Una vez hayamos salido del bucle quiere decir que el analizador léxico ha acabado su función, por lo que lo finalizamos
    finalizarLexico();
}