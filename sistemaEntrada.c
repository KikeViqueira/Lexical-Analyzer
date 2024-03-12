#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "definiciones.h"
#include "sistemaEntrada.h"


//Funciones
void cargarBloque(int bloque);

FILE *codigoFuente;
SistemaDobleCentinela dobleCentinela;

// Funcion que inicializa el sistema de entrada y lee el archivo que se le ha pasado
void initEntrada(char *archivo) {

    // Abrimos el archivo
    if ((codigoFuente = fopen(archivo, "r")) == NULL) {
        perror("No se pudo abrir el archivo");
        exit(EXIT_FAILURE);
    }

    //Al inicializar el sistema de entrada, caragamos el bloque A por convenio
    cargarBloque(1);

}

//Función que inicializa el bloque correspondiente que se le pasa por comandos, guardando en su correspondiente buffer el tamaño indicado del fichero de entrada
void cargarBloque(int bloque){
    if (bloque==1){
        // Leemos el archivo y lo almacenamos en el buffer A, que es el primer bloque. En caso de que en el buffer ya hubiese contenido este se sobreescribe
        fread(dobleCentinela.bufferA, sizeof(char), TAM_BLOQUE-2, codigoFuente);

        printf("Código fuente leído por el bloque A:\n%s\n", dobleCentinela.bufferA);

        //Hacemos que los punteros apunten al inicio del buffer
        dobleCentinela.inicio = dobleCentinela.bufferA;
        dobleCentinela.delantero = dobleCentinela.bufferA;
        dobleCentinela.bufferA[TAM_BLOQUE-1] = EOF; // Añadimos el caracter EOF al final del buffer para saber cuando tenemos que cambiar de bloque

    }else if (bloque == 2){
        // Leemos el archivo y lo almacenamos en el buffer B, que es el segundo bloque
        fread(dobleCentinela.bufferB, sizeof(char), TAM_BLOQUE-2, codigoFuente);

        printf("Código fuente leído por el bloque B:\n%s\n", dobleCentinela.bufferB);

        //Hacemos que los punteros apunten al inicio del buffer
        dobleCentinela.inicio = dobleCentinela.bufferB;
        dobleCentinela.delantero = dobleCentinela.bufferB;
        dobleCentinela.bufferB[TAM_BLOQUE-1] = EOF; // Añadimos el caracter EOF al final del buffer para saber cuando tenemos que cambiar de bloque


    }else{
        printf("Número de bloque ( %d ) incorrecto\n",bloque);
    }


}

//Función que le manda el siguiente caracter al analizador léxico
char siguienteCaracter() {

    char c;
    //Miramos si el puntero delantero apunta a un EOF
    if(*dobleCentinela.delantero == EOF){
        if (dobleCentinela.delantero==dobleCentinela.bufferA+TAM_BLOQUE-1){
            //Apunta al EOF del primer bloque (Buffer A), hacemos que apunte al inicio del segundo bloque
            dobleCentinela.delantero=dobleCentinela.bufferB;

        }else if(dobleCentinela.delantero==dobleCentinela.bufferB+TAM_BLOQUE-1){
            //Apunta al EOF del segundo bloque (buffer B), hacemos que apunte al inicio del primer bloque
            dobleCentinela.delantero=dobleCentinela.bufferA;

        }else{
            //Fin de fichero
            printf("Estamos en el final del fichero\n");
        }
    }
    //Guardamos el valor del puntero delantero sin riesgo de que apunte a un EOF
    c=*dobleCentinela.delantero;

    dobleCentinela.delantero++;//Aumentamos la direccion de memoria en 1
    return c;
}

//Función para retroceder un caracter
void retrocederCaracter() {
    dobleCentinela.delantero--;
}

//Función que reserva la memoria necesaria al lexema del token y le introduce su correspondiente valor
void aceptar(token *componente){
    /*Miramos si el puntero inicio está en el bloque A, para eso lo que hacemos es mirar si apunta a una dirección de memoria válida para un caracter dentro del buffer
     Por lo que miramos desde la posición cero del buffer hasta la penúltima, ya que la última está reservada para EOF*/
    if(dobleCentinela.inicio>=dobleCentinela.bufferA && dobleCentinela.inicio<=dobleCentinela.bufferA + TAM_BLOQUE - 1){
        //Miramos ahora donde está el puntero delantero
        if(dobleCentinela.delantero>=dobleCentinela.bufferA && dobleCentinela.delantero<=dobleCentinela.bufferA + TAM_BLOQUE - 1){
            /**Si los dos punteros están en el mismo buffer, siempre que se llame a esta función el puntero delantero estará delante al puntero inicio
             Para saber los elementos que tiene el lexema que se ha aceptado lo que haremos es restar la posición del puntero delantero menos la del inicio
             Esto nos devuelve la cantidad de elementos desde la posición del puntero inicio (incluída) hasta la del delantero (no inluída), cuando ambos punteros están en una zona de memoria común**/
            size_t longitudLexema= dobleCentinela.delantero - dobleCentinela.inicio;
            componente->lexema=(char*)malloc(sizeof (char)*(longitudLexema+1)); //Tenemos que reservar un sitio más para el caracter nulo

            //copiamos la zona del buffer A al componente.lexema
            memcpy(componente->lexema,dobleCentinela.inicio,longitudLexema);
            //Añadimos al lexema el caracter nulo
            componente->lexema[longitudLexema]='\0';

        }
        else{
            //Puntero delantero está en el bloque B
            size_t longitudA = (dobleCentinela.bufferA+TAM_BLOQUE) - dobleCentinela.inicio; //Devuelve los elementos desde el inicio incluído al EOF excluído
            size_t longitudB = (dobleCentinela.delantero - dobleCentinela.bufferB); //Devuelve los elementos desde el inicio del bloque B hasta la pos anterior al puntero delantero
            size_t longitudTotal = longitudA + longitudB;
            if(longitudTotal>TAM_BLOQUE-1){
                //Se ha superado el tamaó establecido para los lexemas
                printf("Tamaño lexema a aceptar %zu\n",longitudTotal);
            }
            componente->lexema=(char*)malloc(sizeof (char)*(longitudTotal+1));

            //Copiamos del buffer A a componente.lexema
            memcpy(componente->lexema,dobleCentinela.inicio,longitudA);
            //Copiamos del buffer B a componente.lexema
            memcpy(componente->lexema+ longitudA, dobleCentinela.bufferB, longitudB);
            //Añadimos al lexema el caracter nulo
            componente->lexema[longitudTotal]='\0';

        }

    }else{
        //Sabemos que inicio esta en el buffer B
        if(dobleCentinela.delantero>=dobleCentinela.bufferB && dobleCentinela.delantero<=dobleCentinela.bufferB + TAM_BLOQUE - 1){
            size_t longitudLexema= dobleCentinela.delantero - dobleCentinela.inicio;
            componente->lexema=(char*)malloc(sizeof (char)*(longitudLexema+1)); //Tenemos que reservar un sitio más para el caracter nulo

            //copiamos la zona del buffer A al componente.lexema
            memcpy(componente->lexema,dobleCentinela.inicio,longitudLexema);
            //Añadimos al lexema el caracter nulo
            componente->lexema[longitudLexema]='\0';
        }
        else{
            //Puntero delantero está en el bloque A
            size_t longitudB = (dobleCentinela.bufferB+TAM_BLOQUE) - dobleCentinela.inicio;
            size_t longitudA = (dobleCentinela.delantero - dobleCentinela.bufferA);
            size_t longitudTotal = longitudA + longitudB;
            if(longitudTotal>TAM_BLOQUE-1){
                //Se ha superado el tamaó establecido para los lexemas
                printf("Tamaño lexema a aceptar %zu\n",longitudTotal);
            }
            componente->lexema=(char*)malloc(sizeof (char)*(longitudTotal+1));

            //Copiamos del buffer A a componente.lexema
            memcpy(componente->lexema,dobleCentinela.inicio,longitudB);
            //Copiamos del buffer B a componente.lexema
            memcpy(componente->lexema+ longitudB, dobleCentinela.bufferB, longitudA);
            //Añadimos al lexema el caracter nulo
            componente->lexema[longitudTotal]='\0';

        }
    }
    //Una vez hemos aceptado el lexema, lo que hacemos es mover el puntero de inicio a donde está el delantero
    dobleCentinela.inicio=dobleCentinela.delantero;
}

void omitirCaracter(){
    dobleCentinela.delantero++;//Aumentamos la direccion de memoria en 1
}


//Finalizar sistema de entrada (Funcion donde liberaremos el buffer que contiene el contenido del fichero)