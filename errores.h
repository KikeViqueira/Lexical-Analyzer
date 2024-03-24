#ifndef LEXICAL_ANALYZER_ERRORES_H
#define LEXICAL_ANALYZER_ERRORES_H

#define ERROR_FICHERO 0
#define ENTERO_INVALIDO 1
#define FLOTANTE_INVALIDO 2
#define OP_DELI_INVALIDO 3
#define LITERAL_INVALIDO 4
#define ID_INVALIDO 5
#define SUPERA_TAMANHO 6
#define CARACTER_NO_CONOCIDO 7

void reportarError(int estado); //Según el estado que reciba se mostrará un mensaje diciendo lo que ha ocurrido

void lineaError(int c);//Función que nos indicrá en que línea del archivo fuente ha tenido lugar el error

#endif //LEXICAL_ANALYZER_ERRORES_H
