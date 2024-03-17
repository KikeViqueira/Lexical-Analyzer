#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sistemaEntrada.h"


//Funciones
void cargarBloque();

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
    fread(dobleCentinela.bufferA, sizeof(char), TAM_BLOQUE-1, codigoFuente);

    printf("Código fuente leído por el bloque A: %s\n", dobleCentinela.bufferA);

    //Hacemos que los punteros apunten al inicio del buffer
    dobleCentinela.inicio = dobleCentinela.bufferA;
    dobleCentinela.delantero = dobleCentinela.bufferA;
    dobleCentinela.bufferA[TAM_BLOQUE-1] = EOF; // Añadimos el caracter EOF al final del buffer para saber cuando tenemos que cambiar de bloque
    //Ponemos la bandera del siguiente bloque que se cargará
    dobleCentinela.bloque_cargar=1;

}

//Función que inicializa el bloque correspondiente que se le pasa por comandos, guardando en su correspondiente buffer el tamaño indicado del fichero de entrada
void cargarBloque(){

    //Comprobamos si tenemos que cambiar de bloque porque el puntero delantero ya ha procesado el último caracter del buffer en el que está, este tiene que estar apuntando a la última posición de uno de los buffers
    if ((dobleCentinela.delantero == dobleCentinela.bufferA + TAM_BLOQUE -1) || (dobleCentinela.delantero == dobleCentinela.bufferB + TAM_BLOQUE -1)){

        if(dobleCentinela.retroceso==1){//Miramos si tenemos que cargar el bloque al que se va a mover el puntero delantero
            //En este caso no tenemos que cargarlo porque anteriormente ya hemos estado
            if(dobleCentinela.delantero==dobleCentinela.bufferA+TAM_BLOQUE-1){
                //Estamos al final del bufferA y pasamos al B sin cargarlo de nuevo
                dobleCentinela.delantero=dobleCentinela.bufferB;
            }
            else{
                //En otro caso estaremos al final del bufferB por lo que hacemos lo mismo pero al reves
                dobleCentinela.delantero=dobleCentinela.bufferA;
            }
            //Ponemos la flag a cero hasta que se vuelva a poner a 1 debido a un retroceso en la ejecución
            dobleCentinela.retroceso=0;
        }
        else{
            //Miramos el valor del bloque que tenemos que cargar, ya que la flag de retroceso está desactivada
            if(dobleCentinela.bloque_cargar==0){

                // Leemos el archivo y lo almacenamos en el buffer A. En caso de que en el buffer ya hubiese contenido este se sobreescribe
                fread(dobleCentinela.bufferA, sizeof(char), TAM_BLOQUE-1, codigoFuente);

                //-------------------------------------------------------------------------------------------printf("Código fuente leído por el bloque A: %s\n", dobleCentinela.bufferA);

                //Hacemos que el puntero delantero apunte al inicio del buffer
                dobleCentinela.delantero = dobleCentinela.bufferA;
                dobleCentinela.bufferA[TAM_BLOQUE-1] = EOF; // Añadimos el caracter EOF al final del buffer para saber cuando tenemos que cambiar de bloque
                dobleCentinela.bloque_cargar=1;
            }
            else{
                // Leemos el archivo y lo almacenamos en el buffer B, que es el segundo bloque
                fread(dobleCentinela.bufferB, sizeof(char), TAM_BLOQUE-1, codigoFuente);

                //-------------------------------------------------------------------------------------------printf("Código fuente leído por el bloque B: %s\n", dobleCentinela.bufferB);

                //Hacemos que el puntero delantero apunte al inicio del buffer
                dobleCentinela.delantero = dobleCentinela.bufferB;
                dobleCentinela.bufferB[TAM_BLOQUE-1] = EOF; // Añadimos el caracter EOF al final del buffer para saber cuando tenemos que cambiar de bloque
                dobleCentinela.bloque_cargar=0;
            }
        }
    }
}

//Función que le manda el siguiente caracter al analizador léxico
char siguienteCaracter() {
    char c;
    //Guardamos el valor del puntero delantero sin riesgo de que apunte a un EOF
    c=*dobleCentinela.delantero;
    dobleCentinela.delantero++;//Aumentamos la direccion de memoria en 1 de manera segura
    cargarBloque();
    return c;
}

//Función para retroceder un caracter
void retrocederCaracter() {
    /*Tenemos que comprobar si el puntero delantero está al principio de uno de los buffers, porque en ese caso si retrocedemos tenemos que activar
     una bandera para que no nos borre el contenido del último bloque que se ha cargado, ya que cuando volvamos a adelantar el delantero se cargaría de nuevo el bloque y perderíamos la información*/
    if (dobleCentinela.delantero==dobleCentinela.bufferB){
        //Si delantero apunta al principio del bufferB teneos que volver al último caracter antes del EOF del bufferA
        dobleCentinela.delantero=dobleCentinela.bufferA+TAM_BLOQUE-2;
        dobleCentinela.retroceso=1; //Activamos la flag de que vamos a retroceder y cuando volvamos al bloque del que hemos retrocedido no se cargue información, ya que existe información que aún no se ha procesado
    }
    else if(dobleCentinela.delantero==dobleCentinela.bufferA){
        //Si delantero apunta al principio del bufferA teneos que volver al último caracter antes del EOF del bufferB
        dobleCentinela.delantero=dobleCentinela.bufferB+TAM_BLOQUE-2;
        dobleCentinela.retroceso=1;
    }
    else{
        //Si no estamos en ninguno de los casos anteriores es seguro retroceder de caracter
        dobleCentinela.delantero--;
    }
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
            printf("Tamanho del lexema aceptado: %zu\n",longitudLexema);
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
            printf("Tamanho del lexema aceptado: %zu\n",longitudTotal);

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
            size_t longitudB = (dobleCentinela.bufferB+TAM_BLOQUE-1) - dobleCentinela.inicio;
            size_t longitudA = (dobleCentinela.delantero - dobleCentinela.bufferA);
            size_t longitudTotal = longitudA + longitudB;
            printf("Tamanho del lexema aceptado: %zu\n",longitudTotal);

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
    //Aumentamos la direccion de memoria en el puntero inicio, para comezan a procesar el siguiente caracter
    dobleCentinela.inicio++;
}


//Finalizar sistema de entrada (Funcion donde liberaremos el buffer que contiene el contenido del fichero)