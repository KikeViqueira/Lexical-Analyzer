#include <stdio.h>
#include <stdlib.h>
#include "TS.h"
#include "definiciones.h"
#include "abb.h"

TABB arbol;

void initTS(TABB *arbol) {

    //Creamos la estructura base del árbol
    crearAbb(arbol);

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
        insertarElementoAbb(arbol,palabras_reservadas[i]);
    }

    //Imprimimos el árbol para ver si se ha creado bien
    imprimir(*arbol);

}