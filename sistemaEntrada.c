#include <stdio.h>
#include <stdlib.h>

#include "definiciones.h"
#include "sistemaEntrada.h"

//Funciones
void cargarBloque(int longitudFichero);

FILE *codigoFuente;
SistemaDobleCentinela dobleCentinela;

// Funcion que inicializa el sistema de entrada y lee el archivo que se le ha pasado
void initEntrada(char *archivo) {
    // Abrimos el archivo
    if ((codigoFuente = fopen(archivo, "r")) == NULL) {
        perror("No se pudo abrir el archivo");
        exit(EXIT_FAILURE);
    }

    // Nos movemos al final del archivo para saber su tamaño
    dobleCentinela.bufferA[TAM_BLOQUE-1] = EOF; // Añadimos el caracter EOF al final del buffer para saber cauando tenemos que cambiar de bloque

    cargarBloque(TAM_BLOQUE);


}

void cargarBloque(int longitudFichero){
    // Leemos el archivo y lo almacenamos en el buffer
    fread(dobleCentinela.bufferA, sizeof(char), TAM_BLOQUE-2, codigoFuente);

    printf("Código fuente leído:\n%s\n", dobleCentinela.bufferA);

    //Hacemos que los punteros apunten al inicio del buffer
    dobleCentinela.inicio = dobleCentinela.bufferA;
    dobleCentinela.delantero = dobleCentinela.bufferA;

}

char siguienteCaracter() {

    char c = *dobleCentinela.delantero; //Cogemos valor de la direccion de memoria
    dobleCentinela.delantero++;//Aumentamos la direccion de memoria en 1
    //printf("%c\n",c);
    return c;

}

void retrocederCaracter() {

    dobleCentinela.delantero--;
   // dobleCentinela.inicio = dobleCentinela.delantero; //Igualamso las direcciones de memoria

}

void omitirCaracter(){

}


//Finalizar sistema de entrada (Funcion donde liberaremos el buffer que contiene el contenido del fichero)