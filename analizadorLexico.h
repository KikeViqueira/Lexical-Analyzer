#ifndef ANALIZADORLEXICO_H
#define ANALIZADORLEXICO_H

// Estructura para almacenar los tokens y pasarselos al analizador sintáctico
typedef struct {
    char *lexema;
    int numToken;
} token;

// Función para inicializar el analizador léxico
//void initLexico(char *archivo);

// Función para obtener el siguiente token y pasarselo al analizador sintáctico
token* siguienteToken();

// Función para finalizar el analizador léxico
//void finalizarLexico();

#endif