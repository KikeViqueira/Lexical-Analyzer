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
void automataNumerico(char *c, token *componente);
void automataFlotantes(char *c, char *bufferAux,int *indice, int estadoCorrespondiente, token *componente);
void retrocederLexema(char *lexema, int retroceso);
void automataOperadores(char c, token *componente);


token siguienteToken() {
    token componente;
    componente.lexema=NULL;
    componente.numToken=-1;
    int indice =0;
    char c;
    int estado = 0;
    int aceptado=0;
    c = siguienteCaracter();

    while (c != EOF && aceptado==0) {
        switch (estado) {
            case 0://esatado inicial
                if(isalpha(c) || c=='_'){
                    estado=1;
                }
                else if(isdigit(c) || c=='.'){//Si es número o . llamamos al autómata numérico
                    estado=2;
                }else if(c=='\n'){
                    estado=3;
                }else if(c==' '){
                    //Si es un espacio en blanco no lo procesamos
                    c=siguienteCaracter();
                    break;
                }else{
                    estado=4;
                }
                break;

            case 1:
                automataAlphaNumerico(c,&componente);
                //tenemos que buscar el correspondiente número del lexema en la TS, que lo devolverá la función de buscarLexema()
                componente.numToken=buscarLexema(componente.lexema);
                aceptado=1;
                break;

            case 2:
                automataNumerico(&c,&componente);
                if(c=='.'){//Si se ejecuta este if, estamos en el caso de un punto a secas como lexema
                    componente.lexema =(char*)malloc(sizeof (char)*70);
                    componente.lexema[indice++]=c;
                    componente.lexema[indice]='\0';
                    componente.numToken=(int)c;
                }
                aceptado=1;

                break;
            case 3:
                componente.lexema =(char*)malloc(sizeof (char)*70);
                componente.lexema[indice++]='\\';
                componente.lexema[indice++]='n';
                componente.lexema[indice]='\0';
                componente.numToken=(int)c;
                aceptado=1;
                break;
            case 4:
                componente.lexema =(char*)malloc(sizeof (char)*70);
                componente.lexema[indice++]=c;
                componente.lexema[indice]='\0';
                componente.numToken=(int)c;
                aceptado=1;
                break;

        }

    }
    //Caso de que se ha alcanzado el final del fichero
    if(c=='\000'){
        componente.lexema=NULL;
        componente.numToken=EOF;
    }

    //Le mandamos el componente léxico al analizador sintáctico
    return componente;

}

//Autómata que acepta los distintas cadenas alphanuméricas
void automataAlphaNumerico(char c, token *componente) {
    int capacidad = 70; //Tamaño inicial del buffer
    char *bufferAux = (char*)malloc(sizeof (char)*capacidad);
    int indice = 0;
    bufferAux[indice++]=c; //Primero asigna y después incrementa el indice

    while ((c=siguienteCaracter())!=EOF && (isalnum(c) || c=='_')){

        bufferAux[indice++] = c;
    }

    //Si hemos salido del bucle es por un caracter que no aceptamos por lo que retrocedemos el puntero una posición para que en el siguiente componente léxico se lea dicho caracter
    bufferAux[indice++]=c;
    retrocederLexema(bufferAux,1);

    componente->lexema = strdup(bufferAux); // Copia el buffer auxiliar al lexema del token
    free(bufferAux); // Libera el buffer auxiliar
}

//Autómata que acepta los distintos tipos de números enteros
void automataNumerico(char *c, token *componente){
    int aceptadoNumeros=0;

    int capacidad = 70; //Tamaño inicial del buffer
    char *bufferAux = (char*)malloc(sizeof (char)*capacidad);
    int indice = 0;
    //El caracter que recibe este automata es un numero o un punto

    int estado = 0;

    while (aceptadoNumeros == 0){
        switch (estado) {
            case 0:
                //Miramos si teenemos que ir al automáta
                if(*c=='0'){
                    bufferAux[indice++]=*c;
                    estado=1;

                }else if(isdigit(*c)){//Se supone ya que no es cero por la primera comprobracion
                    bufferAux[indice++]=*c;
                    estado=11;
                }else if(*c=='.'){//Este if es para aceptar números del estilo de .0001
                    automataFlotantes(c,bufferAux,&indice,0, componente);
                    aceptadoNumeros=1;
                    break;
                }else{
                    break;
                }
                break;

            case 1:
                //automata de codificaciones (binario, hexadecimal y octal)
                *c=siguienteCaracter();
                bufferAux[indice++]=*c;
                if(*c=='b' || *c=='B'){
                    estado=2;
                }else if (*c=='x' || *c=='X'){
                    estado=8;
                }else if (*c=='o' || *c=='O'){
                    estado=5;
                }else if(*c=='e' || *c=='E') {
                    //Nos vamos al autómata de flotantes al caso en concreto que maneja los posibles números con exponentes
                    bufferAux[indice++] = *c;
                    automataFlotantes(c, bufferAux, &indice, 4, componente);
                    aceptadoNumeros = 1;
                    break;
                }
                else if(*c=='j'||*c=='J'){
                    aceptadoNumeros=1;
                    componente->numToken=IMAGINARIO;
                    break;
                }else if(isdigit(*c)){
                    estado=11;
                }
                else if(*c=='.'){
                    automataFlotantes(c,bufferAux,&indice,1,componente);
                    aceptadoNumeros=1;
                    break;
                }
                else{
                    retrocederLexema(bufferAux,1);
                    aceptadoNumeros=1;
                    componente->numToken=ENTERO;
                    break;
                }
                break;

                //Del case 2 al 4 estamos en la posibilidad de números binarios
            case 2:
                //Estamos en el caso de los binarios
                *c=siguienteCaracter();
                bufferAux[indice++]=*c;

                if(*c=='0'||*c=='1'){
                    estado=3;
                }else{
                    //En caso de que después de la b se reciba un caracter distinto a un punto tenemos que retroceder tres posiciones para que el puntero apunte al cero inicial
                    retrocederLexema(bufferAux,2);
                    aceptadoNumeros=1;
                    componente->numToken=ENTERO;
                    break;
                }
                break;

            case 3:
                while((*c=siguienteCaracter())=='0' || *c=='1'){
                    bufferAux[indice++]=*c;
                }
                if(*c=='_'){
                    bufferAux[indice++]=*c;
                    estado=4;
                }else{
                    bufferAux[indice++]=*c;
                    //Se ha aceptado el lexama, retrocedemos una posicion
                    retrocederLexema(bufferAux,1);
                    aceptadoNumeros=1;
                    componente->numToken=ENTERO;
                    break;
                }
                break;

            case 4:
                *c=siguienteCaracter();
                bufferAux[indice++]=*c;
                if(*c=='0' || *c=='1'){
                    estado=3;
                }else{
                    //Si recibimos otro caracter cualquiera que no sea un número, tenemos que retroceder 3 posiciones para que el puntero apunte al último número y aceptamos el lexema
                    retrocederLexema(bufferAux,2);
                    aceptadoNumeros=1;
                    componente->numToken=ENTERO;
                    break;//Salimos del switch
                }
                break;

                //Del case 5 al 7 estamos en la posibilidad de números octales
            case 5:
                //Estamos en el caso de los binarios
                *c=siguienteCaracter();
                bufferAux[indice++]=*c;

                if(*c>='0' && *c<='7' ){
                    estado=6;
                }else{
                    retrocederLexema(bufferAux,2);
                    aceptadoNumeros=1;
                    componente->numToken=ENTERO;
                    break;
                }
                break;

            case 6:
                while((*c=siguienteCaracter())>='0' && *c<='7' ){
                    bufferAux[indice++]=*c;
                }
                if(*c=='_'){
                    bufferAux[indice++]=*c;
                    estado=7;
                }else{
                    bufferAux[indice++]=*c;
                    //Se ha aceptado el lexama, retrocedemos una posicion
                    retrocederLexema(bufferAux,1);
                    aceptadoNumeros=1;
                    componente->numToken=ENTERO;
                    break;
                }
                break;

            case 7:
                *c=siguienteCaracter();
                bufferAux[indice++]=*c;
                if(*c>='0' && *c<='7'){
                    estado=6;
                }else{
                    //Si recibimos otro caracter cualquiera tenemos que retroceder 2 posiciones y aceptamos el lexema
                    retrocederLexema(bufferAux,2);
                    aceptadoNumeros=1;
                    componente->numToken=ENTERO;
                    break;//Salimos del switch
                }
                break;

                //Del case 8 al 10 estamos en la posibilidad de números hexadecimales
            case 8:
                //Estamos en el caso de los binarios
                *c=siguienteCaracter();
                bufferAux[indice++]=*c;

                if(isxdigit(*c)){
                    estado=9;
                }else{
                    retrocederLexema(bufferAux,2);
                    aceptadoNumeros=1;
                    componente->numToken=ENTERO;
                    break;
                }
                break;

            case 9:
                while(isxdigit(*c=siguienteCaracter())){
                    bufferAux[indice++]=*c;
                }
                if(*c=='_'){
                    bufferAux[indice++]=*c;
                    estado=10;
                }else{
                    bufferAux[indice++]=*c;
                    //Se ha aceptado el lexama, retrocedemos una posicion
                    retrocederLexema(bufferAux,1);
                    aceptadoNumeros=1;
                    componente->numToken=ENTERO;
                    break;
                }
                break;

            case 10:
                *c=siguienteCaracter();
                bufferAux[indice++]=*c;
                if(isxdigit(*c)){
                    estado=9;
                }else{
                    //Si recibimos otro caracter cualquiera tenemos que retroceder 2 posiciones y aceptamos el lexema
                    retrocederLexema(bufferAux,2);
                    aceptadoNumeros=1;
                    componente->numToken=ENTERO;
                    break;//Salimos del switch
                }
                break;


            case 11:
                while(isdigit(*c=siguienteCaracter())){
                    bufferAux[indice++]=*c;
                }

                if(*c=='_'){
                    bufferAux[indice++]=*c;
                    estado=12;
                }
                else if(*c=='.'){
                    automataFlotantes(c,bufferAux,&indice,1,componente);
                    aceptadoNumeros=1;
                    break;
                }
                else if(*c=='j' || *c=='J'){
                    bufferAux[indice++]=*c;
                    aceptadoNumeros=1;
                    componente->numToken=IMAGINARIO;
                    break;//Los números imaginarios obligatoriamente terminan por J
                }else if(*c=='e' || *c=='E'){
                    //Nos vamos al autómata de flotantes al caso en concreto que maneja los posibles números con exponentes
                    bufferAux[indice++]=*c;
                    automataFlotantes(c,bufferAux,&indice,4,componente);
                    aceptadoNumeros=1;
                    break;
                }
                else{
                    bufferAux[indice++]=*c;
                    //Se ha aceptado el lexama, retrocedemos una posicion
                    retrocederLexema(bufferAux,1);
                    aceptadoNumeros=1;
                    componente->numToken=ENTERO;
                    break;
                }
                break;

            case 12:
                *c=siguienteCaracter();
                bufferAux[indice++]=*c;
                if(isdigit(*c)){
                    estado=11;
                }else{
                    //Si recibimos otro caracter cualquiera tenemos que retroceder 2 posiciones y aceptamos el lexema
                    retrocederLexema(bufferAux,2);
                    aceptadoNumeros=1;
                    componente->numToken=ENTERO;
                    break;//Salimos del switch
                }
                break;

        }
    }

    componente->lexema = strdup(bufferAux);
    free(bufferAux);

}

//Autómataque acepta los distintos tipos de números flotantes
void automataFlotantes(char *c, char *bufferAux,int *indice, int estadoCorrespondiente, token *componente){

    /*MUY IMPORTANTE: *indice++: Obtén el valor apuntado por indice, luego incrementa el puntero indice en sí (para que apunte a la siguiente dirección de memoria).
     (*indice)++: Incrementa el valor al que indice apunta, sin cambiar la dirección que almacena el puntero indice.
     En nuestro tenemos que usar la segunda opción ya que indice no es un array dinámico si no una variable tipo inte que funciona de contador, a la cual incrementaremos el valor para
     saber donde hay que almacenar el caracter en el buffer*/

    int aceptadoFlotante=0;

    while (aceptadoFlotante==0){
        switch (estadoCorrespondiente) {

            //CREO QUE EL CERO Y EL UNO SE PUEDEN UNIR EN UNO YA QUE SABEMOS A DONDE VOLVER YA QUE DEPENDE DE DONDE SE HAYA LLAMADO EL SUBAUTOMATA

            case 0:
                bufferAux[(*indice)++]=*c;
                //llegamos siempre con un punto (Caso para aceptar numeros del tipo .2)
                *c=siguienteCaracter();
                bufferAux[(*indice)++]=*c;
                if(isdigit(*c)){
                    estadoCorrespondiente=2;
                }
                else if(*c=='j' || *c=='J'){
                    bufferAux[(*indice)++]=*c;
                    aceptadoFlotante=1;
                    componente->numToken=IMAGINARIO;
                    break;//Los números imaginarios obligatoriamente terminan por J
                }
                else{
                    retrocederLexema(bufferAux,1);
                    componente->numToken=FLOTANTE;
                    aceptadoFlotante=1;
                    break;
                }
                break;

            case 1:
                bufferAux[(*indice)++]=*c;
                //llegamos siempre con un punto (Caso para aceptar números del tipo 35.2)
                *c=siguienteCaracter();
                bufferAux[(*indice)++]=*c;
                if(isdigit(*c)){
                    estadoCorrespondiente=2;
                }
                else if(*c=='j' || *c=='J'){
                    bufferAux[(*indice)++]=*c;
                    aceptadoFlotante=1;
                    componente->numToken=IMAGINARIO;
                    break;//Los números imaginarios obligatoriamente terminan por J
                }
                else{
                    retrocederLexema(bufferAux,1);
                    componente->numToken=FLOTANTE;
                    aceptadoFlotante=1;
                    break;
                }
                break;

            case 2:
                //Mientras lleguen números aceptamos
                while(isdigit(*c=siguienteCaracter())){
                    bufferAux[(*indice)++]=*c;
                }

                if(*c=='_'){
                    bufferAux[(*indice)++]=*c;
                    estadoCorrespondiente=3;
                }
                else if(*c=='j' || *c=='J'){
                    bufferAux[(*indice)++]=*c;
                    aceptadoFlotante=1;
                    componente->numToken=IMAGINARIO;
                    break;//Los números imaginarios obligatoriamente terminan por J
                }
                else if(*c=='e' || *c=='E'){
                    bufferAux[(*indice)++]=*c;
                    estadoCorrespondiente=4;
                }
                else{
                    bufferAux[(*indice)++]=*c;
                    retrocederLexema(bufferAux,1);
                    componente->numToken=FLOTANTE;
                    aceptadoFlotante=1;
                    break;
                }
                break;

            case 3:

                *c=siguienteCaracter();
                bufferAux[(*indice)++]=*c;
                if(isdigit(*c)){
                    estadoCorrespondiente=2;
                }
                else{
                    //Si recibimos otro caracter cualquiera tenemos que retroceder 3 posiciones y aceptamos el lexema
                    retrocederLexema(bufferAux,2);
                    componente->numToken=FLOTANTE;
                    aceptadoFlotante=1;
                    break;//Salimos del switch
                }
                break;

                //Los casos de aqui en adelante son para números con exponentes
            case 4:
                *c=siguienteCaracter();
                bufferAux[(*indice)++]=*c;
                if(isdigit(*c)){
                    estadoCorrespondiente=2;
                }
                else if(*c=='-' || *c=='+'){
                    estadoCorrespondiente=5;
                }
                else{
                    retrocederLexema(bufferAux,2);//retrocedemos el caracter actual y el E anterior
                    componente->numToken=FLOTANTE;
                    aceptadoFlotante=1;
                    break;
                }
                break;

            case 5:
                //Caso por si después del E se recibe un - o +
                *c=siguienteCaracter();
                bufferAux[(*indice)++]=*c;
                if(isdigit(*c)){
                    estadoCorrespondiente=2;
                }
                else{
                    retrocederLexema(bufferAux,3);//retrocedemos el caracter actual, el + o - y el E
                    componente->numToken=FLOTANTE;
                    aceptadoFlotante=1;
                    break;
                }
                break;

        }
    }


}

//Autómata que acepta los distintos tipos de operadores y delimitadores
void automataOperadores(char c, token *componente){

    int capacidad = 70; //Tamaño inicial del buffer
    char *bufferAux = (char*)malloc(sizeof (char)*capacidad);
    int indice = 0;
    bufferAux[indice++]=c; //Primero asigna y después incrementa el indice

    int aceptadoOperador=0,estado=0;

    while (aceptadoOperador==0){
        switch (estado) {
            case 0:
                if(c=='>'){
                    estado=1;
                }else if(c=='<'){
                    estado=2;
                }else if(c=='+'||c=='%'||c=='@'||c=='&'||c=='|'||c=='='||c=='^'||c==':') {
                    estado = 6;
                }else if(c=='-') {
                    estado = 3;
                }else if(c=='('||c=='['||c=='{'||c=='}'||c==']'||c==')'||c==','||c==';'||c=='~') {
                    aceptadoOperador=1;
                }else if(c=='/') {
                    estado = 4;
                }else if(c=='*') {
                    estado = 5;
                }
                break;

            case 1:
                c=siguienteCaracter();
                bufferAux[indice++]=c;
                if(c=='>'){
                    estado=6;
                }else if(c=='='){
                    aceptadoOperador=1;
                }else{
                    //Se rompe el lexema por lo que retrocedemos una posición y aceptamos
                    retrocederLexema(bufferAux,1);
                    aceptadoOperador=1;
                }
                break;

            case 2:
                c=siguienteCaracter();
                bufferAux[indice++]=c;
                if(c=='<'){
                    estado=6;
                }else if(c=='='){
                    aceptadoOperador=1;
                }else{
                    //Se rompe el lexema por lo que retrocedemos una posición y aceptamos
                    retrocederLexema(bufferAux,1);
                    aceptadoOperador=1;
                }
                break;

            case 3:
                c=siguienteCaracter();
                bufferAux[indice++]=c;
                if(c=='=' || c=='>'){
                    aceptadoOperador=1;
                }else{
                    //Se rompe el lexema por lo que retrocedemos una posición y aceptamos
                    retrocederLexema(bufferAux,1);
                    aceptadoOperador=1;
                }
                break;

            case 4:
                c=siguienteCaracter();
                bufferAux[indice++]=c;
                if(c=='/'){
                    estado=6;
                }else if(c=='='){
                    aceptadoOperador=1;
                }else{
                    //Se rompe el lexema por lo que retrocedemos una posición y aceptamos
                    retrocederLexema(bufferAux,1);
                    aceptadoOperador=1;
                }
                break;

            case 5:
                c=siguienteCaracter();
                bufferAux[indice++]=c;
                if(c=='*'){
                    estado=6;
                }else if(c=='='){
                    aceptadoOperador=1;
                }else{
                    //Se rompe el lexema por lo que retrocedemos una posición y aceptamos
                    retrocederLexema(bufferAux,1);
                    aceptadoOperador=1;
                }
                break;

            case 6:
                c=siguienteCaracter();
                bufferAux[indice++]=c;
                if(c=='='){
                    aceptadoOperador=1;
                }else{
                    //Se rompe el lexema por lo que retrocedemos una posición y aceptamos
                    retrocederLexema(bufferAux,1);
                    aceptadoOperador=1;
                }
                break;
        }
    }

    /*Una vez tenemos el lexema, lo que tenemos que conseguir es su número correspondiente
     * En caso de tratarse de un lexema de un solo caracter, su número será su propio valor ascii
     * En caso de ser compuesto, tendremos que comparar el lexema con los posibles operadores y delimitadores compuestos y obtener su correspondiente valor en definiciones.h*/

    //Si el buffer tiene un elemento pasamos ascii, si no comparamos las posibilidades

    componente->lexema=bufferAux;
    free(bufferAux);
}

void retrocederLexema(char *lexema, int retroceso) {
    lexema[strlen(lexema) - retroceso] = '\0'; // Ajustar el fin de la cadena directamente.
    for (int i = 0; i < retroceso; i++) {
        retrocederCaracter();
    }
}



