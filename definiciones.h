#ifndef DEFINICIONES_H
#define DEFINICIONES_H

#define RESET   "\x1b[0m"

#define RED     "\x1b[31m"
// Palabras reservadas cuando el analizador sintáctico imprima los componentes en pantalla mostrará su número con color rojo
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
#define FALSE 288
#define NONE 289
#define TRUE 290
#define CLASS 291
#define CONTINUE 292
#define BREAK 293
#define EXCEPT 294
#define FINALLY 295
#define IS 296
#define LAMBDA 297
#define TRY 298
#define ASSERT 299
#define DEL 300
#define GLOBAL 301
#define NONLOCAL 302
#define WITH 303
#define ASYNC 304
#define AWAIT 305
#define PASS 306
#define RAISE 307
#define YIELD 308

#define MAGENTA "\x1b[35m" //Serán representadas mediante el color magenta
#define ID 309

//Tipo de número reconocido por el analizador léxico
#define BLUE    "\x1b[34m"
#define ENTERO 310
#define GREEN   "\x1b[32m"
#define FLOTANTE 311
#define IMAGINARIO 312 //Los números imaginarios se mostrarán en gris (default) aunque en wilcoxon.py no hay

//Delimitadores y operadores compuestos serán representados con el color amarillo
#define YELLOW  "\x1b[33m"
//Operadores compuestos
#define POTENCIA 313
#define DIVISION_ENTERA 314
#define DESPLAZA_IZQUIERDA 315
#define DESPLAZA_DERECHA 316
#define DOS_PUNTOS_IGUAL 317
#define MENOR_IGUAL 318
#define MAYOR_IGUAL 319
#define IGUAL_IGUAL 320
#define EXCLAMACION_IGUAL 321
//Delimitadores compuestos
#define FLECHA 322
#define MAS_IGUAL 323
#define MENOS_IGUAL 324
#define POR_IGUAL 325
#define DIVIDIDO_IGUAL 326
#define DIVISION_ENTERA_IGUAL 327
#define MODULO_IGUAL 328
#define ARROBA_IGUAL 329
#define AND_IGUAL 330
#define OR_IGUAL 331
#define XOR_BIT_IGUAL 332
#define DESPLAZA_DERECHA_IGUAL 333
#define DESPLAZA_IZQUIERDA_IGUAL 334
#define POTENCIA_IGUAL 335

//Literales
#define CYAN    "\x1b[36m" //Se mostrarán con el color Cyan
#define LITERAL 337


#endif