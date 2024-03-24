#include <malloc.h>
#include <string.h>
#include "TS.h"
#include "definiciones.h"
#include "abb.h"

#define GREEN   "\x1b[32m"
#define RESET   "\x1b[0m"

TABB arbol;

void initTS() {

    //Creamos la estructura base del árbol
    crearAbb(&arbol);

    TIPOELEMENTOABB palabras_reservadas[]= {
        {"import", IMPORT},
        {"from", FROM},
        {"as", AS},
        {"return", RETURN},
        {"def", DEF},
        {"for", FOR},
        {"in", IN},
        {"if", IF},
        {"else", ELSE},
        {"elif", ELIF},
        {"while", WHILE},
        {"and", AND},
        {"or", OR},
        {"not", NOT},
        {"print", PRINT},
        {"False", FALSE},
        {"None", NONE},
        {"True", TRUE},
        {"class", CLASS},
        {"continue", CONTINUE},
        {"break", BREAK},
        {"except", EXCEPT},
        {"finally", FINALLY},
        {"is", IS},
        {"lambda", LAMBDA},
        {"try", TRY},
        {"assert", ASSERT},
        {"del", DEL},
        {"global", GLOBAL},
        {"nonlocal", NONLOCAL},
        {"with", WITH},
        {"async", ASYNC},
        {"await", AWAIT},
        {"pass", PASS},
        {"raise", RAISE},
        {"yield", YIELD},
    };

    int tamanho = sizeof(palabras_reservadas) / sizeof(palabras_reservadas[0]);

    //Insertamos en el árbol las palabras reservadas, cada una en un nodo distinto del árbol
    for (int i=0; i<tamanho; i++){
        char *reservada = (char*) malloc(strlen(palabras_reservadas[i].lexema)+1);
        strcpy(reservada, palabras_reservadas[i].lexema);
        palabras_reservadas[i].lexema = reservada;
        insertarElementoAbb(&arbol,palabras_reservadas[i]);
    }

    //Imprimimos el árbol para ver si se ha creado de manera correcta
    printf(GREEN"\nCONTENIDO INICIAL DE NUESTRA TABLA DE SIMBOLOS: \n"RESET);
    printf(GREEN"----------------------------------------------------------\n"RESET);
    imprimir(arbol);
    printf("\n");

}

void buscarLexema(token *componente){
    //Hacemos una copia del nodo, así aunque se libere la memoria del token en otra parte del código, en el árbol seguiremos con su información
    TIPOELEMENTOABB nodoAux;
    nodoAux.numToken=0;

    //Tenemos que buscar el nodo con el identificador o clave lexema, si no está se llamará a insertarLexema()
    buscarNodoAbb(arbol, componente->lexema, &nodoAux);

    if(nodoAux.numToken==0){
        /*Si se entra aqui es porque la función buscarnodo no ha encontrado ningún nodo cuya clave coincida con el lexema
         que se ha pasado como clave, en el v¡caso de no entrar en el if ya hemos encontrado el nodo y devolvemos número correspondiente al lexema*/
        insertarLexema(componente->lexema, &nodoAux);

    }

    componente->numToken=nodoAux.numToken;

}


void insertarLexema(char *lexema_a_insertar, TIPOELEMENTOABB *nodo){
    //tenemos que hacer una copia del lexema cuando lo metamos en la TS, porque si no cuando el sintáctico hago un free del token este se borrará también de la tabla de símbolos
    nodo->lexema=(char*) malloc(strlen(lexema_a_insertar)+1);
    strcpy(nodo->lexema,lexema_a_insertar);
    nodo->numToken=ID;

    //Ahora que nodo tiene la información necesaria, metemos el elemento en el árbol
    insertarElementoAbb(&arbol,*nodo);
}

void imprimirTablaSimbolos(){
    printf(GREEN"\nCONTENIDO DE NUESTRA TABLA DE SIMBOLOS DESPUES DE PROCESAR EL ARCHIVO: \n"RESET);
    printf(GREEN"----------------------------------------------------------------------------\n"RESET);
    imprimir(arbol);
}

void liberarTS(){
    destruirAbb(&arbol);
}