#include <stdio.h>
#include "TS.h"
#include "definiciones.h"
#include "abb.h"

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
        insertarElementoAbb(&arbol,palabras_reservadas[i]);
    }

    //Imprimimos el árbol para ver si se ha creado de manera correcta
    imprimir(arbol);

}

int buscarLexema(char *lexema_a_buscar){
    TIPOELEMENTOABB nodoAux;
    nodoAux.lexema=NULL;
    nodoAux.numToken=0;

    //Tenemos que buscar el nodo con el identificador o clave lexema, si no está se llamará a insertarLexema()
    buscarNodoAbb(arbol, lexema_a_buscar, &nodoAux);

    if(nodoAux.numToken==0){
        /*Si se entra aqui es porque la función buscarnodo no ha encontrado ningún nodo cuya clave coincida con el lexema
         que se ha pasado como clave, en el v¡caso de no entrar en el if ya hemos encontrado el nodo y devolvemos número correspondiente al lexema*/
        insertarLexema(lexema_a_buscar, &nodoAux);
    }

    //Devolvemos el número correspondiente al token con el lexema que se ha pasado por argumentos
    return nodoAux.numToken;
}


void insertarLexema(char *lexema_a_insertar, TIPOELEMENTOABB *nodo){
    nodo->lexema=lexema_a_insertar;
    nodo->numToken=ID;

    //Ahora que nodo tiene la información necesaria, metemos el elemento en el árbol
    insertarElementoAbb(&arbol,*nodo);
}