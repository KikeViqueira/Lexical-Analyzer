#ifndef ABB_H
#define ABB_H

#include "analizadorLexico.h"

/**
 * Tipo de dato abstracto para arbol binario con clave de
 * ordenacion y elemento de celda.
 */

///////////////////////////////////////INICIO PARTE MODIFICABLE

typedef char * TIPOCLAVE; //Lexema

typedef token TIPOELEMENTOABB;//Estructura componente léxico que se guardará en cada nodo del árbol

//////////////////////////////////////////FIN PARTE MODIFICABLE

typedef struct celda * TABB;//tipo opaco

/////////////////////////////// FUNCIONES

//FUNCIONES DE CREACIÓN Y DESTRUCCIÓN DEL �?RBOL
/**
 * Crea el arbol vacio.
 * @param A Puntero al arbol. Debe estar inicializado.
 */
void crearAbb(TABB *A);

/**
 * Destruye el arbol recursivamente
 * @param A El arbol que queremos destruir
 */
void destruirAbb(TABB *A);

//FUNCIONES DE INFORMACIÓN
/**
 * Comprueba si el arbol esta vacio
 * @param A El arbol binario
 */
unsigned esAbbVacio(TABB A);

/**
 * Devuelve el subarbol izquierdo de A
 * @param A - Arbol original
 */
TABB izqAbb(TABB A);
/**
 * Devuelve el subarbol derecho de A
 * @param A - Arbol original
 */
TABB derAbb(TABB A);

/**
 * Busca un nodo con clave <cl> en el arbol y, si existe
 * modifica el puntero <nodo> para que lo apunte.
 * @param A Arbol binario en el que se busca el nodo.
 * @param cl Clave que se buscara.
 * @param nodo Puntero a un tipoelem.
 */
void buscarNodoAbb(TABB A, TIPOCLAVE cl, TIPOELEMENTOABB *nodo);

//FUNCIONES DE MODIFICACIÓN
/**
 * Inserta un nuevo nodo en el arbol para el elemento E
 * del que toma su clave. Esta clave no debe existir en
 * el arbol.
 * @param A Arbol binario
 * @param E Informacion del nuevo nodo.
 */
void insertarElementoAbb(TABB *A, TIPOELEMENTOABB E);
/**
 * Suprime el nodo del arbol <A>
 * @param A
 * @param E
 */
void suprimirElementoAbb(TABB *A,TIPOELEMENTOABB E);

/**
 * Imprime todos los nodos existentes en el arbol
 * @param A
 */
void imprimir(TABB A);

#endif	// ABB_H


