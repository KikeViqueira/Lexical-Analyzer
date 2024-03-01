#include <stdio.h>
#include <stdlib.h>

#include "definiciones.h"
#include "sistemaEntrada.h"

FILE *codigoFuente;
char *buffer;     // Buffer que almacena el código fuente sin comentarios de texto
int indice = 0;   // Índice actual en el buffer
int longitud = 0; // Longitud total del buffer

// Funcion que inicializa el sistema de entrada y lee el archivo que se le ha pasado
void initEntrada(char *archivo) {
    // Abrimos el archivo
    if ((codigoFuente = fopen(archivo, "r")) == NULL) {
        perror("No se pudo abrir el archivo");
        exit(EXIT_FAILURE);
    }

    // Nos movemos al final del archivo para saber su tamaño
    fseek(codigoFuente, 0, SEEK_END);
    longitud = ftell(codigoFuente);
    // Volvemos al inicio del fichero para comenzar el proceso de lectura
    fseek(codigoFuente, 0, SEEK_SET);

    // Asignamos la memoria para el buffer basandonos en el tamaño del fichero leído
    buffer = (char *)malloc(longitud + 1); //+1 para el caracter nulo
    if (buffer == NULL) {
        perror("Error al asignar memoria para el buffer");
        fclose(codigoFuente);
        exit(EXIT_FAILURE);
    }

    // Leemos el archivo y lo almacenamos en el buffer
    fread(buffer, sizeof(char), longitud, codigoFuente);
    buffer[longitud] = '\0'; // Añadimos el caracter nulo al final del buffer

    printf("Código fuente leído:\n%s\n", buffer);

    // Cerramos el archivo
    fclose(codigoFuente);
}

char siguienteCaracter() {
    if (indice < longitud) {
        return buffer[indice++];
    } else {
        return EOF; // Fin de archivo
    }
}

void retrocederCaracter() {
    indice--;
}

void omitirCaracter(){
    indice++;
}