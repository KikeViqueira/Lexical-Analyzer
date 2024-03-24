#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "analizadorLexico.h"
#include <ctype.h>
#include "sistemaEntrada.h"
#include "TS.h"
#include "definiciones.h"
#include "errores.h"
// Autómatas
void automataAlphaNumerico( char c,token *componente);
void automataNumerico(char c, token *componente);
void automataFlotantes(char c,int estadoCorrespondiente, token *componente);
void automataOperadores(char c, token *componente);
void automataComentariosLinea(char c);
void automataLiterales(char c, token *componente);
//Contador que nos indicará en que línea se ha producido el error
int errorLinea=1;
//Flag especial para saber si el lexema a devolver es un punto a secas "." o un número flotante que comienza por punto .673672
int punto=0;

//Función para modularizar el código y no repetir tantas líneas
void aceptarLexema(token *componente,int tipoLexema, int *flagSwitch){
    aceptar(componente, errorLinea);
    componente->numToken=tipoLexema;
    *flagSwitch=1;
}

//Función para informar de que un numero,operador o literal se ha formado de forma incorrecta pero se devolverá un componente léxico aceptando la parte correcta de este
void lexemaMalFormado(int tipoError){
    reportarError(tipoError);
    lineaError(errorLinea);
}

token* siguienteToken() {
    token *componente = (token*) malloc(sizeof(token));
    componente->lexema=NULL;
    componente->numToken=-1;
    int indice =0;
    char c;
    int estado = 0;
    int aceptado=0;
    c = siguienteCaracter();

    while (c != EOF && aceptado==0) {
        switch (estado) {
            case 0://esatado inicial
                if(isalpha(c) || c=='_') estado=1;//Si es un caracter alfabético o una _ llamamos al autómata alphanumérico
                else if(isdigit(c) || c=='.') estado=2;//Si es número o . llamamos al autómata numérico (Vamos a tener en cuenta el flag de si es punto o número flotante
                else if(c=='\n') estado=3;
                else if(c==' '){
                    //Si es un espacio en blanco no lo procesamos, y pasamos a analizar el siguiente caracter
                    omitirCaracter();
                    c=siguienteCaracter();
                }
                else if(c=='#') estado=4;
                else if(c=='"'||c==39) estado=5;
                else estado=6;
                break;

            case 1:
                automataAlphaNumerico(c,componente);
                //tenemos que buscar el correspondiente número del lexema en la TS, que lo devolverá la función de buscarLexema(), si el lexema está compuesto por mas de un caracter
                if(strlen(componente->lexema)==1) componente->numToken=(int)componente->lexema[0];
                else buscarLexema(componente);
                aceptado=1;
                break;

            case 2:
                automataNumerico(c,componente);
                if(punto==1){//Si se ejecuta este if, estamos en el caso de un punto a secas como lexema y no un número flotante que empieza por punto
                    aceptar(componente,errorLinea);
                    componente->numToken=(int)c;
                    punto=0;//Hacemos un reset de el flag
                }
                aceptado=1;
                break;

            case 3:
                errorLinea++;
                aceptarLexema(componente,(int)c,&aceptado);
                break;

            case 4:
                automataComentariosLinea(c);
                //Pedimos el siguiente caracter y nos vamos al estado cero para que decida que autómata tiene que hacerse cargo de procesar el lexema entrante
                c=siguienteCaracter();
                estado=0;
                break;

            case 5:
                automataLiterales(c, componente);
                if(componente->lexema==NULL){
                    //Si el lexema del componente vale NULL quiere decir que el lexema que ha procesado el autómata es un comentario y no ha creado un componente léxico, se ha omitido el lexema
                    c=siguienteCaracter();//Comenzamos a procesar el siguiente lexema
                    estado=0;//Volvemos al case 0 que decidirá a donde tenemos que ir
                }
                else aceptado=1;
                break;

            case 6:
                automataOperadores(c,componente);
                aceptado=1;
                break;

            default:
                reportarError(CARACTER_NO_CONOCIDO);
                break;
        }
    }
    //Caso de que se ha alcanzado el final del fichero
    if(c==EOF){
        finalizarSistemaEntrada();
        componente->lexema=NULL;
        componente->numToken=EOF;
    }
    //Le mandamos el componente->léxico al analizador sintáctico
    return componente;
}

//Autómata que acepta los distintas cadenas alphanuméricas
void automataAlphaNumerico(char c, token *componente) {
    c=siguienteCaracter();
    while (c!=EOF && (isalnum(c) || c=='_')) c=siguienteCaracter();
    if(c==' ' || c=='\n'){
        //Si hemos salido del bucle es por un caracter que no aceptamos por lo que retrocedemos el puntero una posición para que en el siguiente componente léxico se lea dicho caracter
        retrocederCaracter();
        //Aeceptamos el lexema que está correctamente formado
        aceptar(componente,errorLinea);
    }else{
        //El lexema alphanumerico esta mal formado
        lexemaMalFormado(ID_INVALIDO);
        retrocederCaracter();
        aceptar(componente,errorLinea);
    }
}

//Autómata que acepta los distintos tipos de números enteros
void automataNumerico(char c, token *componente){
    //El caracter que recibe este autómata es un número o un punto

    int aceptadoNumeros=0,estado = 0;
    while (aceptadoNumeros == 0){
        switch (estado) {
            case 0:
                //Miramos si teenemos que ir al automáta
                if(c=='0'){
                    estado=1;
                }else if(isdigit(c)){//Se supone ya que no es cero por la primera comprobracion
                    estado=11;
                }else if(c=='.'){//Este if es para aceptar números del estilo de .0001
                    automataFlotantes(c,0, componente);
                    aceptadoNumeros=1;
                }
                break;

            case 1:
                //automata de codificaciones (binario, hexadecimal y octal)
                c=siguienteCaracter();
                if(c=='b' || c=='B') estado=2;
                else if (c=='x' || c=='X') estado=8;
                else if (c=='o' || c=='O') estado=5;
                else if(c=='e' || c=='E') {
                    //Nos vamos al autómata de flotantes al caso en concreto que maneja los posibles números con exponentes
                    automataFlotantes(c,  4, componente);
                    aceptadoNumeros = 1;
                }
                else if(c=='j'||c=='J') aceptarLexema(componente, IMAGINARIO, &aceptadoNumeros);
                else if(isdigit(c)) estado=11;

                else if(c=='.'){
                    automataFlotantes(c,1,componente);
                    aceptadoNumeros=1;
                }
                else if(c==' ' || c=='\n'){
                    retrocederCaracter();
                    aceptarLexema(componente, ENTERO, &aceptadoNumeros);
                }
                else{
                    lexemaMalFormado(ENTERO_INVALIDO);
                    retrocederCaracter();
                    aceptarLexema(componente, ENTERO, &aceptadoNumeros);
                }
                break;

                //Del case 2 al 4 estamos en la posibilidad de números binarios
            case 2:
                //Estamos en el caso de los binarios
                c=siguienteCaracter();

                if(c=='0'||c=='1') estado=3;
                else{
                    lexemaMalFormado(ENTERO_INVALIDO);
                    retrocederCaracter();
                    aceptarLexema(componente, ENTERO, &aceptadoNumeros);
                }
                break;

            case 3:
                c=siguienteCaracter();

                while(c=='0' || c=='1') c=siguienteCaracter();

                if(c=='_') estado=4;
                else if(c==' ' || c=='\n'){
                    //Se ha aceptado el lexama, retrocedemos una posicion
                    retrocederCaracter();
                    aceptarLexema(componente, ENTERO, &aceptadoNumeros);
                }
                else{
                    lexemaMalFormado(ENTERO_INVALIDO);
                    retrocederCaracter();
                    aceptarLexema(componente, ENTERO, &aceptadoNumeros);
                }
                break;

            case 4:
                c=siguienteCaracter();
                if(c=='0' || c=='1') estado=3;
                else{
                    lexemaMalFormado(ENTERO_INVALIDO);
                    //Tendriamos que ir 2 posiciones para atrás ya que venimos a este estado acabando el numero en _ y eso no se admite
                    retrocederCaracter();
                    retrocederCaracter();
                    aceptarLexema(componente, ENTERO, &aceptadoNumeros);
                }
                break;

                //Del case 5 al 7 estamos en la posibilidad de números octales
            case 5:
                //Estamos en el caso de los octales
                c=siguienteCaracter();

                if(c>='0' && c<='7' ) estado=6;
                else{
                    lexemaMalFormado(ENTERO_INVALIDO);
                    retrocederCaracter();
                    aceptarLexema(componente, ENTERO, &aceptadoNumeros);
                }
                break;

            case 6:
                c=siguienteCaracter();
                while(c>='0' && c<='7') c=siguienteCaracter();

                if(c=='_') estado=7;
                else if(c==' ' || c=='\n'){
                    retrocederCaracter();
                    aceptarLexema(componente, ENTERO, &aceptadoNumeros);
                }
                else{
                    lexemaMalFormado(ENTERO_INVALIDO);
                    retrocederCaracter();
                    aceptarLexema(componente, ENTERO, &aceptadoNumeros);
                }
                break;

            case 7:
                c=siguienteCaracter();
                if(c>='0' && c<='7') estado=6;
                else{
                    lexemaMalFormado(ENTERO_INVALIDO);
                    //Tendriamos que ir 2 posiciones para atrás ya que venimos a este estado acabando el numero en _ y eso no se admite
                    retrocederCaracter();
                    retrocederCaracter();
                    aceptarLexema(componente, ENTERO, &aceptadoNumeros);
                }
                break;

                //Del case 8 al 10 estamos en la posibilidad de números hexadecimales
            case 8:
                //Estamos en el caso de los hexadecimales
                c=siguienteCaracter();

                if(isxdigit(c)) estado=9;
                else{
                    lexemaMalFormado(ENTERO_INVALIDO);
                    retrocederCaracter();
                    aceptarLexema(componente, ENTERO, &aceptadoNumeros);
                }
                break;

            case 9:
                c=siguienteCaracter();
                while(isxdigit(c)) c=siguienteCaracter();

                if(c=='_') estado=10;
                else if(c==' ' || c=='\n'){
                    retrocederCaracter();
                    aceptarLexema(componente, ENTERO, &aceptadoNumeros);
                }
                else{
                    lexemaMalFormado(ENTERO_INVALIDO);
                    retrocederCaracter();
                    aceptarLexema(componente, ENTERO, &aceptadoNumeros);
                }
                break;

            case 10:
                c=siguienteCaracter();
                if(isxdigit(c)) estado=9;
                else{
                    lexemaMalFormado(ENTERO_INVALIDO);
                    //Tendriamos que ir 2 posiciones para atrás ya que venimos a este estado acabando el numero en _ y eso no se admite
                    retrocederCaracter();
                    retrocederCaracter();
                    aceptarLexema(componente, ENTERO, &aceptadoNumeros);
                }
                break;

            case 11:
                c=siguienteCaracter();
                while(isdigit(c)) c=siguienteCaracter();

                if(c=='_') estado=12;
                else if(c=='.'){
                    automataFlotantes(c,1,componente);
                    aceptadoNumeros=1;
                }
                else if(c=='j' || c=='J') aceptarLexema(componente, IMAGINARIO, &aceptadoNumeros); //Los números imaginarios obligatoriamente terminan por J
                else if(c=='e' || c=='E'){
                    //Nos vamos al autómata de flotantes al caso en concreto que maneja los posibles números con exponentes
                    automataFlotantes(c,4,componente);
                    aceptadoNumeros=1;
                }
                else if(c==' ' || c=='\n'){
                    //Se ha aceptado el lexama, retrocedemos una posicion
                    retrocederCaracter();
                    aceptarLexema(componente, ENTERO, &aceptadoNumeros);
                }
                else{
                    lexemaMalFormado(ENTERO_INVALIDO);
                    retrocederCaracter();
                    aceptarLexema(componente, ENTERO, &aceptadoNumeros);
                }
                break;

            case 12:
                c=siguienteCaracter();
                if(isdigit(c)) estado=11;
                else{
                    //Si recibimos otro caracter cualquiera tenemos que retroceder 2 posiciones y aceptamos el lexema
                    retrocederCaracter();
                    aceptarLexema(componente, ENTERO, &aceptadoNumeros);
                }
                break;
        }
    }

}

//Autómata que acepta los distintos tipos de números flotantes
void automataFlotantes(char c,int estadoCorrespondiente, token *componente){

    int aceptadoFlotante=0;
    while (aceptadoFlotante==0){
        switch (estadoCorrespondiente) {

            case 0:
                //llegamos siempre con un punto (Caso para aceptar numeros del tipo .2)
                c=siguienteCaracter();
                if(isdigit(c)) estadoCorrespondiente=2;
                else if(c=='j' || c=='J') aceptarLexema(componente, IMAGINARIO, &aceptadoFlotante);
                else{
                    //en caso de recibir otra cosa tenemos que salir del autómata de números ya que el punto se enviará de manera individual
                    punto=1;//Activamos el flag para que el analizador léxico sepa que el lexema que vamos a aceptar es un punto solo
                    retrocederCaracter();
                    aceptadoFlotante=1;
                }
                break;

            case 1:
                //llegamos siempre con un punto (Caso para aceptar números del tipo 35.2)
                c=siguienteCaracter();
                if(isdigit(c)) estadoCorrespondiente=2;
                else if(c=='j' || c=='J') aceptarLexema(componente, ENTERO, &aceptadoFlotante);
                else{
                    retrocederCaracter();
                    aceptarLexema(componente, FLOTANTE, &aceptadoFlotante);
                }
                break;

            case 2:
                c=siguienteCaracter();
                //Mientras lleguen números aceptamos
                while(isdigit(c)) c=siguienteCaracter();

                if(c=='_') estadoCorrespondiente=3;
                else if(c=='j' || c=='J') aceptarLexema(componente, IMAGINARIO, &aceptadoFlotante);
                else if(c=='e' || c=='E') estadoCorrespondiente=4;
                else if(c==' '|| c=='\n'){
                    retrocederCaracter();
                    aceptarLexema(componente, FLOTANTE, &aceptadoFlotante);
                }
                else{
                    lexemaMalFormado(FLOTANTE_INVALIDO);
                    retrocederCaracter();
                    aceptarLexema(componente, FLOTANTE, &aceptadoFlotante);
                }
                break;

            case 3:
                c=siguienteCaracter();
                if(isdigit(c)) estadoCorrespondiente=2;
                else{
                    lexemaMalFormado(FLOTANTE_INVALIDO);
                    //Si recibimos otro caracter cualquiera tenemos que retroceder 2 posiciones y aceptamos el lexema
                    retrocederCaracter();
                    retrocederCaracter();
                    aceptarLexema(componente, FLOTANTE, &aceptadoFlotante);
                }
                break;

                //Los casos de aqui en adelante son para números con exponentes
            case 4:
                c=siguienteCaracter();
                if(isdigit(c)) estadoCorrespondiente=2;
                else if(c=='-' || c=='+') estadoCorrespondiente=5;
                else{
                    lexemaMalFormado(FLOTANTE_INVALIDO);
                    retrocederCaracter();
                    aceptarLexema(componente, FLOTANTE, &aceptadoFlotante);
                }
                break;

            case 5:
                //Caso por si después del E se recibe un - o +
                c=siguienteCaracter();
                if(isdigit(c)) estadoCorrespondiente=2;
                else{
                    lexemaMalFormado(FLOTANTE_INVALIDO);
                    retrocederCaracter();
                    aceptarLexema(componente, FLOTANTE, &aceptadoFlotante);
                }
                break;
        }
    }
}

//Autómata que acepta los distintos tipos de operadores y delimitadores
void automataOperadores(char c, token *componente){

    int aceptadoOperador=0,estado=0;
    while (aceptadoOperador==0){
        switch (estado) {
            case 0:
                if(c=='>') estado=1;
                else if(c=='<') estado=2;
                else if(c=='+'||c=='%'||c=='@'||c=='&'||c=='|'||c=='='||c=='^'||c==':') estado = 6;
                else if(c=='-') estado = 3;
                else if(c=='('||c=='['||c=='{'||c=='}'||c==']'||c==')'||c==','||c==';'||c=='~') {
                    aceptar(componente,errorLinea);
                    aceptadoOperador=1;
                }else if(c=='/') estado = 4;
                else if(c=='*') estado = 5;
                break;

            case 1:
                c=siguienteCaracter();
                if(c=='>') estado=6;
                else if(c=='='){
                    aceptar(componente,errorLinea);
                    aceptadoOperador=1;
                }else if(c==' '||c=='\n'){
                    retrocederCaracter();
                    aceptar(componente,errorLinea);
                    aceptadoOperador=1;
                }else{
                    lexemaMalFormado(OP_DELI_INVALIDO);
                    //Se rompe el lexema por lo que retrocedemos una posición y aceptamos
                    retrocederCaracter();
                    aceptar(componente,errorLinea);
                    aceptadoOperador=1;
                }
                break;

            case 2:
                c=siguienteCaracter();
                if(c=='<') estado=6;
                else if(c=='='){
                    aceptar(componente,errorLinea);
                    aceptadoOperador=1;
                }else if(c==' '||c=='\n'){
                    retrocederCaracter();
                    aceptar(componente,errorLinea);
                    aceptadoOperador=1;
                }else{
                    lexemaMalFormado(OP_DELI_INVALIDO);
                    retrocederCaracter();
                    aceptar(componente,errorLinea);
                    aceptadoOperador=1;
                }
                break;

            case 3:
                c=siguienteCaracter();
                if(c=='=' || c=='>') aceptar(componente,errorLinea);
                else if(c==' '||c=='\n'){
                    retrocederCaracter();
                    aceptar(componente,errorLinea);
                }else{
                    lexemaMalFormado(OP_DELI_INVALIDO);
                    retrocederCaracter();
                    aceptar(componente,errorLinea);
                }
                aceptadoOperador=1;

                break;

            case 4:
                c=siguienteCaracter();
                if(c=='/') estado=6;
                else if(c=='='){
                    aceptar(componente,errorLinea);
                    aceptadoOperador=1;
                }else if(c==' '||c=='\n'){
                    retrocederCaracter();
                    aceptar(componente,errorLinea);
                    aceptadoOperador=1;
                }else{
                    lexemaMalFormado(OP_DELI_INVALIDO);
                    retrocederCaracter();
                    aceptar(componente,errorLinea);
                    aceptadoOperador=1;
                }
                break;

            case 5:
                c=siguienteCaracter();
                if(c=='*') estado=6;
                else if(c=='='){
                    aceptar(componente,errorLinea);
                    aceptadoOperador=1;
                }else if(c==' '||c=='\n'){
                    retrocederCaracter();
                    aceptar(componente,errorLinea);
                    aceptadoOperador=1;
                }else{
                    lexemaMalFormado(OP_DELI_INVALIDO);
                    retrocederCaracter();
                    aceptar(componente,errorLinea);
                    aceptadoOperador=1;
                }
                break;

            case 6:
                c=siguienteCaracter();
                if(c=='=') aceptar(componente,errorLinea);
                else if(c==' '||c=='\n'){
                    retrocederCaracter();
                    aceptar(componente,errorLinea);
                }else{
                    lexemaMalFormado(OP_DELI_INVALIDO);
                    retrocederCaracter();
                    aceptar(componente, errorLinea);
                }
                aceptadoOperador=1;
                break;
        }
    }
    /*Una vez tenemos el lexema, lo que tenemos que conseguir es su número correspondiente.
     * En caso de tratarse de un lexema de un solo caracter, su número será su propio valor ascii.
     * En caso de ser compuesto, tendremos que comparar el lexema con los posibles operadores y delimitadores compuestos y obtener su correspondiente valor en definiciones.h*/

    //Si el buffer tiene un elemento pasamos ascii, si no comparamos las posibilidades.
    //Para saber si tiene un solo elemento usamos strlen() que devuelve los elementos del buffer sin en el caracter nulo '\0'.
    if (strlen(componente->lexema)==1) componente->numToken=(int)componente->lexema[0];
    else{
        //El lexema esta formado por más de dos caracteres, miramos las posibilidades
        if(strcmp(componente->lexema,"**")==0) componente->numToken=POTENCIA;
        else if(strcmp(componente->lexema,"//")==0) componente->numToken=DIVISION_ENTERA;
        else if(strcmp(componente->lexema,"<<")==0) componente->numToken=DESPLAZA_IZQUIERDA;
        else if(strcmp(componente->lexema,">>")==0) componente->numToken=DESPLAZA_DERECHA;
        else if(strcmp(componente->lexema,":=")==0) componente->numToken=DOS_PUNTOS_IGUAL;
        else if(strcmp(componente->lexema,"<=")==0) componente->numToken=MENOR_IGUAL;
        else if(strcmp(componente->lexema,">=")==0) componente->numToken=MAYOR_IGUAL;
        else if(strcmp(componente->lexema,"==")==0) componente->numToken=IGUAL_IGUAL;
        else if(strcmp(componente->lexema,"!=")==0) componente->numToken=EXCLAMACION_IGUAL;
        else if(strcmp(componente->lexema,"->")==0) componente->numToken=FLECHA;
        else if(strcmp(componente->lexema,"+=")==0) componente->numToken=MAS_IGUAL;
        else if(strcmp(componente->lexema,"-=")==0) componente->numToken=MENOS_IGUAL;
        else if(strcmp(componente->lexema,"*=")==0) componente->numToken=POR_IGUAL;
        else if(strcmp(componente->lexema,"/=")==0) componente->numToken=DIVIDIDO_IGUAL;
        else if(strcmp(componente->lexema,"//=")==0) componente->numToken=DIVISION_ENTERA_IGUAL;
        else if(strcmp(componente->lexema,"%=")==0) componente->numToken=MODULO_IGUAL;
        else if(strcmp(componente->lexema,"@=")==0) componente->numToken=ARROBA_IGUAL;
        else if(strcmp(componente->lexema,"&=")==0) componente->numToken=AND_IGUAL;
        else if(strcmp(componente->lexema,"|=")==0) componente->numToken=OR_IGUAL;
        else if(strcmp(componente->lexema,"^=")==0) componente->numToken=XOR_BIT_IGUAL;
        else if(strcmp(componente->lexema,">>=")==0) componente->numToken=DESPLAZA_DERECHA_IGUAL;
        else if(strcmp(componente->lexema,"<<=")==0) componente->numToken=DESPLAZA_IZQUIERDA_IGUAL;
        else componente->numToken=POTENCIA_IGUAL;
    }
}

//Autómata que descarta los comentarios de una linea
void automataComentariosLinea(char c){
    c=siguienteCaracter();
    while((c)!='\n') c=siguienteCaracter(); //Procesamos todos los caracteres hasta encontrar un \n que nos indique el final del comentario de una línea
    retrocederCaracter(); //Retrocedemos el '\n'
    omitirLexema();//Como no nos interesa hacer un componente lexico de un comentario omitimos el lexema
}

//Autómata que acepta los distintos literales posibles de Python
void automataLiterales(char c, token *componente){
    //El caracter que recibe este autómata es una comilla simple ' o una doble comilla "
    int aceptadoLiterales=0;
    int estado = 0;

    while(aceptadoLiterales==0){
        switch (estado) {
            case 0:
                if(c=='"') estado=1;
                else estado=8;//Si no es comilla doble tiene que ser obligatoriamente comilla simple
                break;

            case 1:
                c=siguienteCaracter();
                if(c=='"') estado=2;
                else estado=7;
                break;

            case 2:
                c=siguienteCaracter();
                if(c=='"') estado=3;
                else if(c==' '||c=='\n'){
                    retrocederCaracter();
                    aceptarLexema(componente,LITERAL,&aceptadoLiterales);
                }else{
                    lexemaMalFormado(LITERAL_INVALIDO);
                    retrocederCaracter();
                    aceptarLexema(componente,LITERAL,&aceptadoLiterales);
                }
                break;

            case 3:
                c=siguienteCaracter();
                if(c!='"') estado=4;
                else estado=5;//Caso de tener un string literal multilinea vacío de comilla doble
                break;

            case 4:
                c=siguienteCaracter();
                while(c!='"'){
                    if(c=='\n') errorLinea++;
                    c=siguienteCaracter();
                }
                //Una vez que hemos salido del bucle es porque hemos recibido una doble comilla "
                estado=5;
                break;

            case 5:
                c=siguienteCaracter();
                if(c=='"') estado=6;
                //else estado=4;
                else{
                    retrocederCaracter();
                    aceptarLexema(componente,LITERAL,&aceptadoLiterales);
                }
                break;

            case 6:
                c=siguienteCaracter();
                if(c=='"'){
                    omitirLexema();//Omitimos el lexema ya que es considerado un comentario multilínea
                    aceptadoLiterales=1;
                    //Aceptamos el lexema sin retrocederCaracter ya que no hace falta
                }
                //else estado=4;
                else{
                    retrocederCaracter();
                    aceptarLexema(componente,LITERAL,&aceptadoLiterales);
                }
                break;

            case 7:
                c=siguienteCaracter();
                while(c!='"'){
                    if(c=='\n') errorLinea++;
                    c=siguienteCaracter();
                }
                //Una vez que hemos salido del bucle es porque hemos recibido una doble comilla " y se cierra el String literal de una linea -> "Hola que tal?"
                aceptarLexema(componente,LITERAL,&aceptadoLiterales);
                //Aceptamos el lexema sin retrocederCaracter ya que no hace falta
                break;

            case 8:
                c=siguienteCaracter();
                if(c==39) estado=9;
                else estado=14;//Estamos ante el caso de una abertura de string literal de una linea de simple comilla -> 'H
                break;

            case 9:
                c=siguienteCaracter();
                if(c==39) estado=10;
                else if(c==' '||c=='\n'){
                    retrocederCaracter();
                    aceptarLexema(componente,LITERAL,&aceptadoLiterales);
                }else{
                    lexemaMalFormado(LITERAL_INVALIDO);
                    retrocederCaracter();
                    aceptarLexema(componente,LITERAL,&aceptadoLiterales);
                }
                break;

            case 10:
                c=siguienteCaracter();
                if(c!=39) estado=11;
                else estado=12;//Caso de tener un string literal multilinea vacío de comilla simple
                break;

            case 11:
                c=siguienteCaracter();
                while(c!=39){
                    if(c=='\n') errorLinea++;
                    c=siguienteCaracter();
                }
                //Una vez que hemos salido del bucle es porque hemos recibido una doble simple '
                estado=12;
                break;

            case 12:
                c=siguienteCaracter();
                if(c==39) estado=13;
                //else estado=11;
                else{
                    retrocederCaracter();
                    aceptarLexema(componente,LITERAL,&aceptadoLiterales);
                }
                break;

            case 13:
                c=siguienteCaracter();
                if(c==39){
                    omitirLexema();
                    aceptadoLiterales=1;
                    //Aceptamos el lexema sin retrocederCaracter ya que no hace falta
                }
                //else estado=11;

                else{
                    retrocederCaracter();
                    aceptarLexema(componente,LITERAL,&aceptadoLiterales);
                }
                break;

            case 14:
                c=siguienteCaracter();
                while(c!=39){
                    if(c=='\n') errorLinea++;
                    c=siguienteCaracter();
                }
                //Una vez que hemos salido del bucle es porque hemos recibido una doble comilla " y se cierra el String literal de una linea -> 'Hola que tal?'
                aceptarLexema(componente,LITERAL,&aceptadoLiterales);
                //Aceptamos el lexema sin retrocederCaracter ya que no hace falta
                break;
        }
    }
}


