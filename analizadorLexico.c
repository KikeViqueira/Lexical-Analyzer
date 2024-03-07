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
void automataFlotantes(char c, char *bufferAux,int *indice,int *capacidad, int estadoCorrespondiente);
void retrocederLexema(char *lexema, int retroceso,int *indice);
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

    int capacidad = 70; //Tamaño inicial del buffer
    char *bufferAux = (char*)malloc(sizeof (char)*capacidad);
    int indice = 0;

    if ((c = siguienteCaracter()) != EOF) {
        switch (estado) {
            case 0://esatado inicial
                if(isalpha(c) || c=='_'){
                    automataAlphaNumerico(c,&componente);
                    estado=0;
                }
                else if(isdigit(c) || c=='.'){//Si es número o . llamamos al autómata numérico
                    automataNumerico(c, &componente);
                    estado=0;
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
            //Miramos si teenemos que ir al automáta
            if(c=='0'){
                bufferAux[indice++]=c;
                estado=1;

            }else if(isdigit(c)){//Se supone ya que no es cero por la primera comprobracion
                bufferAux[indice++]=c;
                estado=11;
            }else if(c=='.'){//Este if es para aceptar números del estilo de .0001
                bufferAux[indice++]=c;
                automataFlotantes(c,bufferAux,&indice,&capacidad,0);
            }else{
                break;
            }

        case 1:
            //automata de codificaciones (binario, hexadecimal y octal)
            c=siguienteCaracter();
            bufferAux[indice++]=c;
            if(c=='b' || c=='B'){
                estado=2;
            }else if (c=='x' || c=='X'){
                estado=5;
            }else if (c=='o' || c=='O'){
                estado=8;
            }else if(c=='j'||c=='J'){
                estado=13;
            }else if(isdigit(c)){
                estado=11;
            }
            else{
                retrocederLexema(bufferAux,1,&indice);
                break;
            }

        //Del case 2 al 4 estamos en la posibilidad de números binarios
        case 2:
            //Estamos en el caso de los binarios
            c=siguienteCaracter();
            bufferAux[indice++]=c;

            if(c=='0'||c=='1'){
                estado=3;
            }else{
                retrocederLexema(bufferAux,2,&indice);
                break;
            }

        case 3:
            while((c=siguienteCaracter())=='0' || c=='1'){
                bufferAux[indice++]=1;
            }
            if(c=='_'){
                bufferAux[indice++]=1;
                estado=4;
            }else{
                //Se ha aceptado el lexama, retrocedemos una posicion
                retrocederLexema(bufferAux,1,&indice);
                break;
            }

        case 4:
            c=siguienteCaracter();
            bufferAux[indice++]=c;
            if(c=='0' || c=='1'){
                estado=3;
            }else{
                //Si recibimos otro caracter cualquiera tenemos que retroceder 2 posiciones y aceptamos el lexema
                retrocederLexema(bufferAux,2,&indice);
                break;//Salimos del switch
            }

        //Del case 5 al 7 estamos en la posibilidad de números octales
        case 5:
            //Estamos en el caso de los binarios
            c=siguienteCaracter();
            bufferAux[indice++]=c;

            if(c>='0' && c<='7' ){
                estado=6;
            }else{
                retrocederLexema(bufferAux,2,&indice);
                break;
            }

        case 6:
            while((c=siguienteCaracter())>='0' && c<='7' ){
                bufferAux[indice++]=1;
            }
            if(c=='_'){
                bufferAux[indice++]=1;
                estado=7;
            }else{
                //Se ha aceptado el lexama, retrocedemos una posicion
                retrocederLexema(bufferAux,1,&indice);
                break;
            }

        case 7:
            c=siguienteCaracter();
            bufferAux[indice++]=c;
            if(c>='0' && c<='7'){
                estado=6;
            }else{
                //Si recibimos otro caracter cualquiera tenemos que retroceder 2 posiciones y aceptamos el lexema
                retrocederLexema(bufferAux,2,&indice);
                break;//Salimos del switch
            }

        //Del case 8 al 10 estamos en la posibilidad de números hexadecimales
        case 8:
            //Estamos en el caso de los binarios
            c=siguienteCaracter();
            bufferAux[indice++]=c;

            if(isxdigit(c)){
                estado=9;
            }else{
                retrocederLexema(bufferAux,2,&indice);
                break;
            }

        case 9:
            while(isxdigit(c=siguienteCaracter())){
                bufferAux[indice++]=1;
            }
            if(c=='_'){
                bufferAux[indice++]=1;
                estado=10;
            }else{
                //Se ha aceptado el lexama, retrocedemos una posicion
                retrocederLexema(bufferAux,1,&indice);
                break;
            }

        case 10:
            c=siguienteCaracter();
            bufferAux[indice++]=c;
            if(isxdigit(c)){
                estado=9;
            }else{
                //Si recibimos otro caracter cualquiera tenemos que retroceder 2 posiciones y aceptamos el lexema
                retrocederLexema(bufferAux,2,&indice);
                break;//Salimos del switch
            }


        case 11:
            while(isdigit(c=siguienteCaracter())){
                bufferAux[indice++]=c;
            }

            if(c=='_'){
                bufferAux[indice++]=1;
                estado=12;
            }else{
                //Se ha aceptado el lexama, retrocedemos una posicion
                retrocederLexema(bufferAux,1,&indice);
                break;
            }

        case 12:
            c=siguienteCaracter();
            bufferAux[indice++]=c;
            if(isdigit(c)){
                estado=11;
            }else{
                //Si recibimos otro caracter cualquiera tenemos que retroceder 2 posiciones y aceptamos el lexema
                retrocederLexema(bufferAux,2,&indice);
                break;//Salimos del switch
            }


            //En caso de numeros flotantes se encargará de ellos el autómata de flotantes


    }

    bufferAux[indice]='\0';
    componente->lexema = strdup(bufferAux);
    free(bufferAux);

}



void automataFlotantes(char c, char *bufferAux,int *indice,int *capacidad, int estadoCorrespondiente){

    switch (estadoCorrespondiente) {

        //CREO QUE EL CERO Y EL UNO SE PUEDEN UNIR EN UNO YA QUE SABEMOS A DONDE VOLVER YA QUE DEPENDE DE DONDE SE HAYA LLAMADO EL SUBAUTOMATA

        case 0:
            //llegamos siempre con un punto (Caso para aceptar numeros del tipo .2)
            c=siguienteCaracter();
            bufferAux[*indice++]=c;
            if(isdigit(c)){
                estadoCorrespondiente=2;
            }else{
                retrocederLexema(bufferAux,1,indice);
                break;
            }

        case 1:
            //llegamos siempre con un punto (Caso para aceptar números del tipo 35.2)
            c=siguienteCaracter();
            bufferAux[*indice++]=c;
            if(isdigit(c)){
                estadoCorrespondiente=2;
            }else{
                retrocederLexema(bufferAux,1,indice);
                break;
            }

        case 2:
            //Mientras lleguen números aceptamos
            while(isdigit(c=siguienteCaracter())){
                bufferAux[*indice++]=c;
            }

            if(c=='_'){
                bufferAux[*indice++]=c;
                estadoCorrespondiente=3;
            }else{
                retrocederLexema(bufferAux,1,indice);
                break;
            }

        case 3:

            c=siguienteCaracter();
            bufferAux[*indice++]=c;
            if(isdigit(c)){
                estadoCorrespondiente=2;
            } else{
                //Si recibimos otro caracter cualquiera tenemos que retroceder 2 posiciones y aceptamos el lexema
                retrocederLexema(bufferAux,2,indice);
                break;//Salimos del switch
            }

        //Los casos de aqui en adelante son para números con exponentes
        case 4:
            break;

        case 5:
            break;
        
    }
}



void retrocederLexema(char *lexema, int retroceso,int *indice) {
    lexema[strlen(lexema) - retroceso] = '\0'; // Ajustar el fin de la cadena directamente.
    for (int i = 0; i < retroceso; i++) {
        retrocederCaracter();
    }
    *indice-=retroceso;
}



