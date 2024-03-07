#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "analizadorLexico.h"
#include <ctype.h>
#include "sistemaEntrada.h"
#include "TS.h"
#include "definiciones.h"

// Autómatas
void automataAlphaNumerico( char c,token *componente);
void automataCodificaciones(char c, char *buffer);
void automataNumerico(char c, token *componente);
void automataFlotantes(char c, char *bufferAux,int *indice,int *capacidad);
void automataExponentes(char c, char *bufferAux,int *indice,int *capacidad);

// Definimos la función de incialización del analizador léxico
void initLexico(char *archivo) {
    // llamamos al inicilizador del sistema de entrada pasándole el archivo por referencia
    initEntrada(archivo);
}

// Autómata alphanumérico
token siguienteToken() {
    token componente;
    componente.lexema = NULL;

    char c;
    int estado = 0;

    int capacidad = 7; //Tamaño inicial del buffer
    char *bufferAux = (char*)malloc(sizeof (char)*capacidad);
    int indice = 0;

    if ((c = siguienteCaracter()) != EOF) {
        switch (estado) {
            case 0://esatado inicial
                if(isalpha(c) || c=='_'){
                    automataAlphaNumerico(c,&componente);
                    estado=0;
                }
                else if(isdigit(c)){
                    automataNumerico(c, &componente);
                    estado=0;
                }
                else if(c=='.'){
                    c=siguienteCaracter();
                    if(isdigit(c)){
                        automataFlotantes(c,bufferAux,&indice,&capacidad);
                    }else{
                        //Se envia el punto asi tal cual
                    }
                }


        }

    }

    return componente;

}

void automataAlphaNumerico(char c, token *componente) {
    int capacidad = 70; //Tamaño inicial del buffer
    char *bufferAux = (char*)malloc(sizeof (char)*capacidad);
    int indice = 0;
    //El primer caracter de la cadena ya es procesado por el switch q elige el autómata correspondiente
    bufferAux[indice++]=c; //Primero asigna y ps incrementa el indice

    while ((c=siguienteCaracter())!=EOF && (isalnum(c) || c=='_')){

        bufferAux[indice++] = c;
    }

    if (c != EOF && c!=' ') {
        retrocederCaracter(); // El último carácter leído no forma parte del token
    }

    bufferAux[indice] = '\0'; // Termina la cadena
    componente->lexema = strdup(bufferAux); // Copia el buffer auxiliar al lexema del token
    free(bufferAux); // Libera el buffer auxiliar
}

//Autómata que acepta números enteros
void automataNumerico(char c, token *componente){
    int capacidad = 70; //Tamaño inicial del buffer
    char *bufferAux = (char*)malloc(sizeof (char)*capacidad);
    int indice = 0;
    //El caracter que recibe este automata es un numero o un punto

    int estado = 0;

    switch (estado) {
        case 0:
            //Automata de formatos de codificación (binario, hexadecimal, octal)
            if(c=='0'){
                bufferAux[indice++]=c;
                automataCodificaciones(c,bufferAux);

            }else{
                estado=1;
            }

        case 1:

            //Estado que representa el automata de números enteros
            bufferAux[indice++]=c;
            //llamamos al automata de enteros
            while(isdigit(c=siguienteCaracter()) || c=='_'){
                bufferAux[indice++]=c;
                if (c=='_' && ((c=siguienteCaracter())=='_')) {
                    retrocederCaracter();
                    break; //Si el siguiente caracter al _ es otro _ el lexema no se puede aceptar
                }
            }

            if(c=='.'){
                //llamamos al automata de números flotantes
                automataFlotantes(c,bufferAux,&indice,&capacidad);
            }else if(c=='E' || c=='e'){
                //llamamos al automatade números con exponentes
                automataExponentes(c,bufferAux,&indice,&capacidad);
            }else{
                //retrocedemos para tener el lexema bien formado
                retrocederCaracter();
            }

            break;

    }

    bufferAux[indice]='\0';
    componente->lexema = strdup(bufferAux);
    free(bufferAux);

}

/**Autómata que reconoce los distintos tipos de condificación (Binario, hexadecimal y octal)**/
void automataCodificaciones(char c, char *buffer){
    int capacidad = 70; //Tamaño inicial del buffer
    char *bufferAux = (char*)malloc(sizeof (char)*capacidad);
    int indice = 0;

    //Para que sea aceptado por este automata tiene que ser siguiente caracter una B b x X o O
    c=siguienteCaracter();

    int estado=0;

    if(c=='b' || c=='B') estado=0;
    else if(c=='o' || c=='O') estado=1;
    else if(c=='x' || c=='X') estado=2;
    else estado=3;

    switch (estado) {

        case 0:
            bufferAux[indice++]=c;
            //caso para el binario
            while((c=siguienteCaracter())=='0' ||c=='1'||c=='_'){

                if(c=='_' && (siguienteCaracter())=='_'){
                    //No puede haber dos barra bajas juntas, por lo que salimos del if y retrocedemos para volver a la primera barra baja
                    retrocederCaracter();
                    break;
                }

            }
            //Salimos del switch
            break;



        case 1:
            //Caso de números octales
            //Solo se aceptan los dígitos del 0 al 7 y las barra bajas
            while((c=siguienteCaracter())>='0' && c<='7' || c=='_'){

                if(c=='_' && (siguienteCaracter())=='_'){
                    //No puede haber dos barra bajas juntas, por lo que salimos del if y retrocedemos para volver a la primera barra baja
                    retrocederCaracter();
                    break;
                }

            }

            break;


        case 2:
            //Caso de números hexadecimales

            //Los caracteres aceptados son 0-9, a-z, A-Z
            while(isxdigit(c=siguienteCaracter())){

                //ARREGLAR LOS IFS QUE ESTAN ASIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII

                if(c=='_' && (siguienteCaracter())=='_'){
                    //No puede haber dos barra bajas juntas, por lo que salimos del if y retrocedemos para volver a la primera barra baja
                    retrocederCaracter();
                    break;
                }
            }
            break;

    }

    if (c != EOF && c!=' ') { //En el caso de que sea un espacio o fin de fila lo que vamos hacer es no procesarlo
        retrocederCaracter(); // El último carácter leído no forma parte del token
    }

    bufferAux[indice]='\0';

} //ARREGLARRRR


void automataFlotantes(char c, char *bufferAux,int *indice,int *capacidad){

}

void automataExponentes(char c, char *bufferAux,int *indice,int *capacidad){

}



