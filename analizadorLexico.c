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
void automataCodificaciones(char c, token *componente);
void automataNumerico(char c, token *componente);
void automataFlotantes(char c, char *bufferAux,int *indice,int *capacidad);

//Funciones
void aumentarMemoria(char *buffer, int *capacidad);//Función para aumentar memoria a un buffer si este se queda sin ella para almacenar un lexema

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
    if ((c = siguienteCaracter()) != EOF) {
        switch (estado) {
            case 0:
                if (c == ' ' || c == '\n' || c == '\t') {
                    estado = 0;
                } else if (isalpha(c) || c == '_') {
                    // Si el caracter es una letra o un guión bajo, quien se encargará de procesar la cadena será el autómata de cadenas alfoanuméricas
                    automataAlphaNumerico(c, &componente);
                    //buscamos en la tabla de simbolos si existe el lexema para saber su numero de token y se lo añadimos al acmpo correspondiente del componente
                    componente.numToken = buscarLexema(componente.lexema);
                    //Devolvemos el componente al analizador sintactico
                    return componente;


                } else if (c=='0' && ((c=siguienteCaracter())=='o' || c=='O' || c=='x' || c=='X' || c=='b' || c=='B')) {
                    // Si el caracter es un cero y su siguiente es un una letra que haga referencia a una codificacion, quien se encargará de procesar la cadena será el autómata de codificaciones
                    automataCodificaciones(c, &componente);
                    //buscamos en la tabla de simbolos si existe el lexema para saber su numero de token y se lo añadimos al acmpo correspondiente del componente
                    componente.numToken = buscarLexema(componente.lexema);
                    //Devolvemos el componente al analizador sintactico
                    return componente;

                }
                else {
                    retrocederCaracter();//Anulamos el efecto del siguiente caracter del else if ya que no se ha procesado por el autómata
                    estado = 1;
                }

            case 1:
                if (isdigit(c)){
                    //Puede ser tanto un entero como un flotante asi que por ahora no lo sabemos


                }

                break;

            default:
                printf("No se ha podido reconocer el caracter por ningun automata\n");
                break;
        }

    }

}

void automataAlphaNumerico(char c, token *componente) {
    int capacidad = 7; //Tamaño inicial del buffer
    char *bufferAux = (char*)malloc(sizeof (char)*capacidad);
    int indice = 0;
    //El primer caracter de la cadena ya es procesado por el switch q elige el autómata correspondiente
    bufferAux[indice++]=c; //Primero asigna y ps incrementa el indice

    while ((c=siguienteCaracter())!=EOF && (isalnum(c) || c=='_')){
        if (indice + 1 >= capacidad) { // +1 para el carácter nulo
            aumentarMemoria(bufferAux,&capacidad);
        }
        bufferAux[indice++] = c;
    }

    if (c != EOF && c!=' ') {
        retrocederCaracter(); // El último carácter leído no forma parte del token
    }

    bufferAux[indice] = '\0'; // Termina la cadena
    componente->lexema = strdup(bufferAux); // Copia el buffer auxiliar al lexema del token
    free(bufferAux); // Libera el buffer auxiliar
}


/**Autómata que reconoce los distintos tipos de condificación (Binario, hexadecimal y octal)**/
void automataCodificaciones(char c, token *componente){
    int capacidad = 7; //Tamaño inicial del buffer
    char *bufferAux = (char*)malloc(sizeof (char)*capacidad);
    int indice = 0;
    //El caracter que recibe este automata es la letra que define que tipo de codificación vamos a reconocer
    bufferAux[indice++]=c; //Primero asigna y ps incrementa el indice

    //Para lllamar a este autómata se tuvo q haber leído un cero y su siguiente caracter una letra ('o','O','b','B','x','X')
    int estado=0;

    switch (estado) {
        //caso para el binario
        case 0:
            if (c=='b' || c=='B'){
                while((c=siguienteCaracter())=='0' ||c=='1'||c=='_'){
                    if (indice + 1 >= capacidad) { // +1 para el carácter nulo
                        aumentarMemoria(bufferAux,&capacidad);
                    }
                    if(c=='_' && (siguienteCaracter())=='_'){
                        //No puede haber dos barra bajas juntas, por lo que salimos del if y retrocedemos para volver a la primera barra baja
                        retrocederCaracter();
                        break;
                    }

                }
                //Salimos del switch
                break;

            }else{
                estado=1;
            }

        case 1:
            //Caso de números octales
            if (c=='o' || c=='O'){
                //Solo se aceptan los dígitos del 0 al 7 y las barra bajas
                while((c=siguienteCaracter())>='0' && c<='7' || c=='_'){

                    if (indice + 1 >= capacidad) { // +1 para el carácter nulo
                        aumentarMemoria(bufferAux,&capacidad);
                    }

                    if(c=='_' && (siguienteCaracter())=='_'){
                        //No puede haber dos barra bajas juntas, por lo que salimos del if y retrocedemos para volver a la primera barra baja
                        retrocederCaracter();
                        break;
                    }

                }

                break;

            }else{
                estado=2;
            }
        case 2:
            //Caso de números hexadecimales
            if (c=='x' || c=='X'){
                //Los caracteres aceptados son 0-9, a-z, A-Z
                while(isxdigit(c=siguienteCaracter())){
                    //Comprobación de si tenemos memoria suficiente
                    if (indice + 1 >= capacidad) { // +1 para el carácter nulo
                        aumentarMemoria(bufferAux,&capacidad);
                    }

                    //ARREGLAR LOS IFS QUE ESTAN ASIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII

                    if(c=='_' && (siguienteCaracter())=='_'){
                        //No puede haber dos barra bajas juntas, por lo que salimos del if y retrocedemos para volver a la primera barra baja
                        retrocederCaracter();
                        break;
                    }
                }
                break;
            }
    }

    if (c != EOF && c!=' ') { //En el caso de que sea un espacio o fin de fila lo que vamos hacer es no procesarlo
        retrocederCaracter(); // El último carácter leído no forma parte del token
    }

    bufferAux[indice]='\0';
    componente->lexema= strdup(bufferAux);
    free(bufferAux);

} //ARREGLARRRR


//Autómata que acepta números enteros
void automataNumerico(char c, token *componente){
    int capacidad = 7; //Tamaño inicial del buffer
    char *bufferAux = (char*)malloc(sizeof (char)*capacidad);
    int indice = 0;
    //El caracter que recibe este automata es un numero o un punto CAMBIARRRRRRRRRRRRRRRRRRRRRR
    bufferAux[indice++]=c; //Primero asigna y despues incrementa el indice

    while(isdigit(c=siguienteCaracter()) || c=='_'){
        //Mientras leamos digitos estaremos en este autómata
        if (indice + 1 >= capacidad) { // +1 para el carácter nulo
            aumentarMemoria(bufferAux,&capacidad);
        }

        if(c=='_' && ((c=siguienteCaracter())=='_') || c!=' '){
            //No puede haber dos barra bajas juntas, por lo que salimos del if y retrocedemos para volver a la primera barra baja
            retrocederCaracter();
            break;
        } else{
            //En el caso de no entrar en el if tenemos que retroceder para recuperrar el caracter que hemos leido y nose ha procesado
            retrocederCaracter();
            bufferAux[indice]=c;//Es seguro meter c
        }

    }

    if(c=='.' || c=='e' || c=='E'){
        automataFlotantes(c,bufferAux,&indice,&capacidad);
        componente->lexema = strdup(bufferAux); // Copia el buffer auxiliar al lexema del token
        free(bufferAux); // Libera el buffer auxiliar
    }else{
        if(c=='j' || c=='J'){//En el caso de que el último caraceter leído sea una 'j' o 'J' estamos ante un número imaginario
            bufferAux[indice++]=c;
        }else if (c != EOF && c!=' '){
            retrocederCaracter(); //El último caracter no pertenece a este token
        }
        bufferAux[indice]='\0';
        componente->lexema = strdup(bufferAux); // Copia el buffer auxiliar al lexema del token
        free(bufferAux); // Libera el buffer auxiliar
    }
}


void automataFlotantes(char c, char *bufferAux,int *indice,int *capacidad){

    //El caracter que recibe este automata es un punto o una e/E
    bufferAux[*indice++]=c;

    int estado=0;

    switch (estado) {
        case 0:
            //caso de 'e' o 'E'
            if(c=='e'||c=='E' && ((c=siguienteCaracter())=='+' || c=='-' || isdigit(c))){
                aumentarMemoria(bufferAux,capacidad);
                bufferAux[*indice++]=c;
                //Solo se puede aceptar el lexema si el siguiente caracter al 'e' o 'E' es un número, un '+' o un '-'
                while(isdigit(c=siguienteCaracter())){
                    aumentarMemoria(bufferAux,capacidad);
                    bufferAux[*indice++]=c;
                }

                if(c=='j' || c=='J'){//En el caso de que el último caraceter leído sea una 'j' o 'J' estamos ante un número imaginario
                    bufferAux[*indice++]=c;
                }else if(c != EOF && c!=' '){
                    retrocederCaracter(); //El último caracter no pertenece a este token
                }
                break;

            }else{
                //retrocedemos ya que el siguiente que hemos leído no ha sido procesado
                retrocederCaracter();
                estado=1;
            }

        case 1:
            //Caso de '.'
            if(c=='.' && (isdigit(c=siguienteCaracter()) || c=='j' || c=='J')){
                aumentarMemoria(bufferAux,capacidad);
                bufferAux[*indice++]=c;

                if(c!='j' && c!='J'){//Si se ha leido una J ya el lexema tiene que acabar ahi
                    while(isdigit(c=siguienteCaracter()) || c=='_'){

                        if (indice + 1 >= capacidad) { // +1 para el carácter nulo
                            aumentarMemoria(bufferAux,capacidad);
                        }

                        if(c=='_' && ((c=siguienteCaracter())=='_') || c!=' '){
                            //No puede haber dos barra bajas juntas, por lo que salimos del if y retrocedemos para volver a la primera barra baja
                            retrocederCaracter();
                            break;
                        } else{
                            //En el caso de no entrar en el if tenemos que retroceder para recuperrar el caracter que hemos leido y nose ha procesado
                            retrocederCaracter();
                            bufferAux[*indice]=c;//Es seguro meter c
                        }

                    }

                    //Comprobar si estamos en el caso de un flotante que se eleva: 3.14e-10
                    if(c=='e'||c=='E' && ((c=siguienteCaracter())=='+' || c=='-' || isdigit(c))){
                        if (indice + 1 >= capacidad) { // +1 para el carácter nulo
                            aumentarMemoria(bufferAux,capacidad);
                        }
                        bufferAux[*indice++]=c;
                        //Vamos para el estado 0 para seguri procesando el lexema
                        estado=0;
                        retrocederCaracter();
                    }

                    //Comprobar si es un flotante imaginario
                    if(c=='j' || c=='J'){//En el caso de que el último caraceter leído sea una 'j' o 'J' estamos ante un número imaginario
                        bufferAux[*indice++]=c;
                    }else if(c != EOF && c!=' '){
                        retrocederCaracter(); //El último caracter no pertenece a este token
                    }
                    break;

                }
            }
    }
    if (c != EOF && c!=' ') { //En el caso de que sea un espacio o fin de fila lo que vamos hacer es no procesarlo
        retrocederCaracter(); // El último carácter leído no forma parte del token
    }
    //tenemos el buffer ya con el lexema correcto asi que volvemos al numérico
    bufferAux[*indice]='\0';
}

//Funciones para modularizar el código
void aumentarMemoria(char *buffer, int *capacidad) {
    *capacidad *= 2;
    char *aumento = realloc(buffer, *capacidad * sizeof(char));
    if (aumento == NULL) {
        printf("Error al hacer realloc\n");
        free(buffer);
        exit(EXIT_FAILURE);
    }
    buffer = aumento;
}

