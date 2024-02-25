#ifndef DEFINICIONES_H
#define DEFINICIONES_H

// Componentes léxicos
#define IMPORT 273
#define FROM 274
#define AS 275
#define RETURN 276
#define DEF 277
#define FOR 278
#define IN 279
#define IF 280
#define ELSE 281
#define ELIF 282
#define WHILE 283
#define AND 284
#define OR 285
#define NOT 286
#define PRINT 287

// Estructura para la tabla de símbolos
typedef struct {
    int componenteLexico;
    char *lexema;
} EntradaTablaSimbolos;

// Definición de la tabla de símbolos como un array de estructuras
#define MAX_ENTRADAS_TS 256
EntradaTablaSimbolos tablaSimbolos[MAX_ENTRADAS_TS];

#endif