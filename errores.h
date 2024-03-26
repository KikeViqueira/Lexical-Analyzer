#ifndef LEXICAL_ANALYZER_ERRORES_H
#define LEXICAL_ANALYZER_ERRORES_H

#define ERROR_FICHERO 0
#define ENTERO_INVALIDO 1
#define FLOTANTE_INVALIDO 2
#define ID_INVALIDO 3
#define SUPERA_TAMANHO 4
#define CARACTER_NO_CONOCIDO 5

void reportarError(int estado); //Según el estado que reciba se mostrará un mensaje diciendo lo que ha ocurrido

void lineaError(int c);//Función que nos indicará en que línea del archivo fuente ha tenido lugar el error

#endif //LEXICAL_ANALYZER_ERRORES_H
