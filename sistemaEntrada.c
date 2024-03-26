#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sistemaEntrada.h"
#include "errores.h"
//Funciones
void cargarBloque();

//Variables globales
FILE *codigoFuente;
SistemaDobleCentinela dobleCentinela;
int errorLinea=1;//Contador que nos indicará en que línea se ha producido el error
int leerLinea=1;//Flag que nos indicará cuando estamos leyendo un \n válido y no uno repetido

// Funcion que inicializa el sistema de entrada y lee el archivo que se le ha pasado
void initEntrada(char *archivo) {

    // Abrimos el archivo
    if ((codigoFuente = fopen(archivo, "r")) == NULL) {
        reportarError(ERROR_FICHERO);
        exit(EXIT_FAILURE);
    }

    //Al inicializar el sistema de entrada, caragamos el bloque A por convenio
    fread(dobleCentinela.bufferA, sizeof(char), TAM_BLOQUE-1, codigoFuente);

    //Hacemos que los punteros apunten al inicio del buffer
    dobleCentinela.inicio = dobleCentinela.bufferA;
    dobleCentinela.delantero = dobleCentinela.bufferA;
    dobleCentinela.bufferA[TAM_BLOQUE-1] = EOF; // Añadimos el caracter EOF al final del buffer para saber cuando tenemos que cambiar de bloque
    //Ponemos la bandera del siguiente bloque que se cargará
    dobleCentinela.bloque_cargar=1;
}

//Función que inicializa el bloque correspondiente que se le pasa por comandos, guardando en su correspondiente buffer el tamaño indicado del fichero de entrada
void cargarBloque(){
    //Si estamos en el caso de que el bloque que vamos a cargar es el 0 -> BufferA, miramos si el inicio está en el.
    if(dobleCentinela.bloque_cargar==0){
        if(dobleCentinela.inicio>=dobleCentinela.bufferA && dobleCentinela.inicio<=dobleCentinela.bufferA + TAM_BLOQUE - 1){
            dobleCentinela.inicio=NULL;//Ponemos en null el puntero inicio ya que si entra el programa en esta sección de código estamos procesando un lexema que ya ha cargado más de dos bloques
        }

        // Leemos el archivo y lo almacenamos en el buffer A. En caso de que en el buffer ya hubiese contenido este se sobreescribe
        size_t caracteresLeidos = fread(dobleCentinela.bufferA, sizeof(char), TAM_BLOQUE-1, codigoFuente);

        //Hacemos que el puntero delantero apunte al inicio del buffer
        dobleCentinela.delantero = dobleCentinela.bufferA;
        dobleCentinela.bufferA[TAM_BLOQUE-1] = EOF; // Añadimos el caracter EOF al final del buffer para saber cuando tenemos que cambiar de bloque
        dobleCentinela.bloque_cargar=1;//Sigueinte bloque que será cargado cuando se procesen todos los caracteres / Bloque 1 -> BufferB

        if(caracteresLeidos < TAM_BLOQUE-1) {
            dobleCentinela.bufferA[caracteresLeidos] = EOF;//EOF correspondiente al final del archivo que estamos procesando
        }
    }
    //Lo mismo que antes pero estando en el bufferA y el siguiente bloque a cargar es el 1 -> BufferB
    else{
        if(dobleCentinela.inicio>=dobleCentinela.bufferB && dobleCentinela.inicio<=dobleCentinela.bufferB + TAM_BLOQUE - 1){
            dobleCentinela.inicio=NULL;
        }
        // Leemos el archivo y lo almacenamos en el buffer B, que es el segundo bloque
        size_t caracteresLeidos = fread(dobleCentinela.bufferB, sizeof(char), TAM_BLOQUE-1, codigoFuente);

        //Hacemos que el puntero delantero apunte al inicio del buffer
        dobleCentinela.delantero = dobleCentinela.bufferB;
        dobleCentinela.bufferB[TAM_BLOQUE-1] = EOF; // Añadimos el caracter EOF al final del buffer para saber cuando tenemos que cambiar de bloque
        dobleCentinela.bloque_cargar=0;
        if(caracteresLeidos < TAM_BLOQUE-1) {
            dobleCentinela.bufferB[caracteresLeidos] = EOF;
        }
    }
}

//Función que le manda el siguiente caracter al analizador léxico
char siguienteCaracter() {
    char c;
    if ((dobleCentinela.delantero == dobleCentinela.bufferA + TAM_BLOQUE -1) || (dobleCentinela.delantero == dobleCentinela.bufferB + TAM_BLOQUE -1)){//Si el puntero delantero está apuntando al EOF de alguno de los buffers llamamos a cargarBloque()
        cargarBloque();
    }

    //Guardamos el valor del puntero delantero sin riesgo de que apunte a un EOF
    c=*dobleCentinela.delantero;
    dobleCentinela.delantero++;//Aumentamos la direccion de memoria en 1 de manera segura

    if(c=='\n' && leerLinea==1) errorLinea++;

    //Si el flag leer estaba en cero y no contamos el \n como devuelto ya podemos poner el flag a 1 para que el siguiente \n si se cuente como linea
    leerLinea=1;

    return c;
}

//Función para retroceder un caracter
void retrocederCaracter() {
    dobleCentinela.delantero--;
    if(*dobleCentinela.delantero=='\n') leerLinea=0; //Activamos el falg indicandole al sistema de entrada que cuando devuelva este \n en siguienteCaracter no lo tenga en cuenta como una linea del archivo
}

//Función para omitir un caracter o lexema
void omitirCaracter_lexema(){
    //Igualamos la dirección de memoria de inicio con la de delantero, que ya estará apuntando al caracter siguiente a devolver
    dobleCentinela.inicio=dobleCentinela.delantero;
}

//Función para informar de que un numero,operador o literal se ha formado de forma incorrecta pero se devolverá un componente léxico aceptando la parte correcta de este
void lexemaMalFormado(int tipoError){
    reportarError(tipoError);
    lineaError(errorLinea);
}

//Función que reserva la memoria necesaria al lexema del token y le introduce su correspondiente valor
void aceptar(token *componente){

    if(dobleCentinela.inicio==NULL){
        //Si inicio vale null es porque se ha cargado dos veces el bloque donde está inicio procesando el mismo lexema, y como es lógico se ha superado el tamaño de lexema
        if(dobleCentinela.bloque_cargar==0){//Si el siguiente bloque que se va a cargar es el A sabemos que tenemos el puntero delantero en el bufferB
            size_t longitudB = (dobleCentinela.delantero - dobleCentinela.bufferB);
            if(longitudB == TAM_BLOQUE-1){//Delantero está ya en la última posición del bufferB (EOF)
                componente->lexema=(char*)malloc(sizeof (char)*(TAM_BLOQUE));
                memcpy(componente->lexema, &dobleCentinela.bufferB,TAM_BLOQUE-1);

            }else{
                //Si no tenemos que coger los caracteres restantes para llegar al tamaño de lexema en el bufferA
                size_t longitudA = (TAM_BLOQUE - 1) - longitudB;//Tamaño lexema menos los caracteres que se han recogido en B

                lexemaMalFormado(SUPERA_TAMANHO);//Indicamos en que línea del fichero fuente ha tenido lugar el lexema que ha superado el tamaño

                componente->lexema=(char*)malloc(sizeof (char)*(TAM_BLOQUE));//Le pasamos directamente este valor ya que para la reserva hay que usar TAM_BLOQUE-1 + 1 por el caracter nulo

                //Copiamos del buffer A a componente.lexema
                memcpy(componente->lexema, &dobleCentinela.bufferA[TAM_BLOQUE-1-longitudA],longitudA);
                //Copiamos del buffer B a componente.lexema
                memcpy(componente->lexema+longitudA, dobleCentinela.bufferB, longitudB);
                //Añadimos al lexema el caracter nulo
                componente->lexema[TAM_BLOQUE-1]='\0';
            }

        }else {
            //El caso contrario, el bloque que se va a cargar es el B, y sabemos que el puntero delantero está en el bufferA
            size_t longitudA = (dobleCentinela.delantero - dobleCentinela.bufferA);
            if(longitudA == TAM_BLOQUE-1){
                componente->lexema=(char*)malloc(sizeof (char)*(TAM_BLOQUE));
                memcpy(componente->lexema, &dobleCentinela.bufferA,TAM_BLOQUE-1);

            }else{
                size_t longitudB = (TAM_BLOQUE-1) - longitudA;
                lexemaMalFormado(SUPERA_TAMANHO);
                componente->lexema=(char*)malloc(sizeof (char)*(TAM_BLOQUE));
                memcpy(componente->lexema, &dobleCentinela.bufferB[TAM_BLOQUE-1-longitudB],longitudB);
                memcpy(componente->lexema+longitudB, dobleCentinela.bufferA, longitudA);
                componente->lexema[TAM_BLOQUE-1]='\0';
            }
        }

    }
    //Aqui manejamos las posibilidades que existen donde hemos aceptado un lexema el cual no se pasa del tamaño máximo de lexema
    else{
        /*Miramos si el puntero inicio está en el bloque A, para eso lo que hacemos es mirar si apunta a una dirección de memoria válida para un caracter dentro del buffer
     Por lo que miramos desde la posición cero del buffer hasta la penúltima, ya que la última está reservada para EOF*/
        if(dobleCentinela.inicio>=dobleCentinela.bufferA && dobleCentinela.inicio<=dobleCentinela.bufferA + TAM_BLOQUE - 1){
            //Miramos ahora donde está el puntero delantero
            if(dobleCentinela.bloque_cargar==1){//El siguiente bloque que se va a cargar es el 1 por lo que sabemos que delantero está en el bufferA
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
                size_t longitudA = (dobleCentinela.bufferA+TAM_BLOQUE-1) - dobleCentinela.inicio; //Devuelve los elementos desde el inicio incluído al EOF excluído
                size_t longitudB = (dobleCentinela.delantero - dobleCentinela.bufferB); //Devuelve los elementos desde el inicio del bloque B hasta la pos anterior al puntero delantero
                size_t longitudTotal = longitudA + longitudB;

                if(longitudTotal>TAM_BLOQUE-1){
                    //Se ha superado el tamanho establecido para los lexemas
                    lexemaMalFormado(SUPERA_TAMANHO);
                    longitudA= (TAM_BLOQUE-1)-longitudB;
                    componente->lexema=(char*)malloc(sizeof (char)*(TAM_BLOQUE));
                    memcpy(componente->lexema, &dobleCentinela.bufferA[TAM_BLOQUE-1-longitudA],longitudA);
                    memcpy(componente->lexema+longitudA, dobleCentinela.bufferB, longitudB);
                    componente->lexema[TAM_BLOQUE-1]='\0';
                }else{

                    componente->lexema=(char*)malloc(sizeof (char)*(longitudTotal+1));
                    //Copiamos del buffer A a componente.lexema
                    memcpy(componente->lexema,dobleCentinela.inicio,longitudA);
                    //Copiamos del buffer B a componente.lexema
                    memcpy(componente->lexema+ longitudA, dobleCentinela.bufferB, longitudB);
                    //Añadimos al lexema el caracter nulo
                    componente->lexema[longitudTotal]='\0';
                }
            }

        }else{
            //Sabemos que inicio esta en el buffer B
            if(dobleCentinela.bloque_cargar==0){//El siguiente bloque que se va a cargar es el 0 por lo que sabemos que el puntero delantero está en el bufferB
                size_t longitudLexema= dobleCentinela.delantero - dobleCentinela.inicio;
                componente->lexema=(char*)malloc(sizeof (char)*(longitudLexema+1)); //Tenemos que reservar un sitio más para el caracter nulo

                //copiamos la zona del buffer A al componente.lexema
                memcpy(componente->lexema,dobleCentinela.inicio,longitudLexema);
                //Añadimos al lexema el caracter nulo
                componente->lexema[longitudLexema]='\0';
            }
            else{
                //Puntero delantero está en el bloque A
                size_t longitudB = (dobleCentinela.bufferB+TAM_BLOQUE-1) - dobleCentinela.inicio;
                size_t longitudA = (dobleCentinela.delantero - dobleCentinela.bufferA);
                size_t longitudTotal = longitudA + longitudB;

                if(longitudTotal>TAM_BLOQUE-1){
                    //Se ha superado el tamaó establecido para los lexemas
                    lexemaMalFormado(SUPERA_TAMANHO);
                    longitudB= (TAM_BLOQUE-1)-longitudA;
                    componente->lexema=(char*)malloc(sizeof (char)*(TAM_BLOQUE));
                    memcpy(componente->lexema, &dobleCentinela.bufferB[TAM_BLOQUE-1-longitudB],longitudB);
                    memcpy(componente->lexema+longitudB, dobleCentinela.bufferA, longitudA);
                    componente->lexema[TAM_BLOQUE-1]='\0';
                }else{
                    componente->lexema=(char*)malloc(sizeof (char)*(longitudTotal+1));

                    //Copiamos del buffer A a componente.lexema
                    memcpy(componente->lexema,dobleCentinela.inicio,longitudB);
                    //Copiamos del buffer B a componente.lexema
                    memcpy(componente->lexema+ longitudB, dobleCentinela.bufferA, longitudA);
                    //Añadimos al lexema el caracter nulo
                    componente->lexema[longitudTotal]='\0';
                }
            }
        }
    }
    //Una vez hemos aceptado el lexema, lo que hacemos es mover el puntero de inicio a donde está el delantero
    dobleCentinela.inicio=dobleCentinela.delantero;
}

//Finalizar sistema de entrada (Funcion donde liberaremos el buffer que contiene el contenido del fichero)
void finalizarSistemaEntrada(){
    //Tenemos que cerrar el archivo y liberar los recursos usados por el sistema de entrada
    fclose(codigoFuente);
}

