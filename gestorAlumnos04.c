/************************************************************
Autor: Grupo 4
Fecha: 15/06/2020
Asignatura: Estructura de Datos y Algoritmos, curso 19/20
************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#define CAPACITY 3000 // Tamaño de la tabla Hash
#define DATA_FILE "alumnos.txt"
#define SEPARATOR ";"
 
unsigned long funcionHash(char* str) {
    unsigned long i = 0;
    for (int j=0; str[j]; j++)
        i += str[j];
    return i % CAPACITY;
}
 
typedef struct Ht_item Ht_item;
 
/**
 * Definición de cada item de la tabla Hash.
 */
struct Ht_item {
    int* id;
    char* nombre;
    char* matriculado;
    char* telefono;
    char* email;
};
 
typedef struct HashTable HashTable;
 
/**
 * Se define la tabla hash.
 */
struct HashTable {
    // Contiene un array de punteros a items
    Ht_item** items;
    int size;
    int count;
};

HashTable* ht = NULL;
 
/*
 * Método que recibe los parámetros para crear un item y lo retorna.
 */
Ht_item* creatarItem(int* id, char* nomb, char* mtr, char* tlf, char* email) {
    // Se crea un puntero al nuevo item de la tabla
    Ht_item* item = (Ht_item*) malloc (sizeof(Ht_item));
    item->id = (int*) malloc (strlen(id) + 1);
    item->nombre = (char*) malloc (strlen(nomb) + 1);
    item->matriculado = (char*) malloc (strlen(mtr) + 1);
    item->telefono = (char*) malloc (strlen(tlf) + 1);
    item->email = (char*) malloc (strlen(email) + 1);

    strcpy(item->id, id);
    strcpy(item->nombre, nomb);
    strcpy(item->matriculado, mtr);
    strcpy(item->telefono, tlf);
    strcpy(item->email, email);

    return item;
}
 
/**
 * Método que crea la tabla del tamaño recibido como parámetro.
 */
HashTable* create_table(int tam) {
    HashTable* tabla = (HashTable*) malloc (sizeof(HashTable));
    tabla->size = tam;
    tabla->count = 0;
    tabla->items = (Ht_item**) calloc (tabla->size, sizeof(Ht_item*));
    for (int i=0; i<tabla->size; i++)
        tabla->items[i] = NULL;
 
    return tabla;
}
 
/**
 * Libera espacio del memoria del item recibido.
 */
void liberaItem(Ht_item* item) {
    free(item->id);
    free(item->nombre);
    free(item->matriculado);
    free(item->telefono);
    free(item->email);
    free(item);
}
 
/**
 * Libera espacio de memoria utilizado por la tabla recibida por parámetro.
 */
void liberaTabla(HashTable* tabla) {
    for (int i=0; i<tabla->size; i++) {
        Ht_item* item = tabla->items[i];
        if (item != NULL)
            liberaItem(item);
    }
 
    free(tabla->items);
    free(tabla);
}

/**
 * Función para insertar un elemento o timen en l atabla hash.
 */
int insertarItem(HashTable* tabla, int* id, char* nomb, char* mtr, char* tlf, char* email) {

    //Se valida que la tabla no esté llena
    if (tabla->count == tabla->size) {
        printf("Error: Tabla llena.\n");
        return EXIT_FAILURE;
    }
    
    // Se crea el item
    Ht_item* item = creatarItem(id, nomb, mtr, tlf, email);
 
    // Se genera el índice
    unsigned long indice = funcionHash(id);
 
    Ht_item* itemActual = tabla->items[indice];
    if (itemActual == NULL) {
        // Id no existe.
        if (tabla->count == tabla->size) {
            // Tabla hash llena
            printf("Error al insertar: Tabla Hash está llena.\n");
            // Liberamos el espacio del item creado.
            liberaItem(item);
            return EXIT_FAILURE;
        }
         
        // Inserta directamente
        tabla->items[indice] = item;
        tabla->count++;
    }
    else {
        // Escenario de colisión
        printf("Error: no se puede insertar alumno con ID existente: %d\n", *id);
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
 
/**
 *Función para buscar en l atabla recibida por identificador de elemento.
 */
char* buscar(HashTable* table, int* id) {
    // Se obtiene la clave con la función hash.
    int indice = funcionHash(id);
    Ht_item* item = table->items[indice];
 
    // Nos aseguramos que se mueven los item que no son nulos
    while (item != NULL) {
        if (strcmp(item->id, id) == 0)
            return item->nombre;
    }
    return NULL;
}
 
/**
 *Función para borrar de l atabla recibida el elemento que coincida con el identificador recibido.
 */
char borrarAlumno(HashTable* table, int* id) {
    int index = funcionHash(id);
    Ht_item* item = table->items[index];

    if (item == NULL) {
        // Si item no existe
        printf("No existe alumno para el ID: %d\n", *id);
        return EXIT_FAILURE;

    }
    else {
        if (strcmp(item->id, id) == 0) {
            // Sin cadena de colisión. Eliminar el artículo y establece el índice de la tabla en NULL
            table->items[index] = NULL;
            liberaItem(item);
            table->count--;
            printf("Alumno con ID: %d borrado.\n", *id);
            return EXIT_SUCCESS;
        }
    }
    
    return EXIT_FAILURE;
}

int editarHt(HashTable* table, int* key, char* mtr) {
    // Se obtiene la clave con la función hash.
    int indice = funcionHash(key);
    Ht_item* item = table->items[indice];
 
    // Nos aseguramos que se mueven los item que no son nulos
    // Comprobamos que los valores de la función que vamos a actualizar tienen un contenido valido
    while (item != NULL) {
        if (strcmp(item->id, key) == 0){
            if(mtr != NULL || mtr != "")
                strcpy(item->matriculado, mtr);
            return EXIT_SUCCESS;
        }
            
    }
    return EXIT_FAILURE;
}

/**
 * Método para guardar el conttenido de la tabla hash en el fichero externo.
 */
void guardaFile( HashTable* tabla ){

    FILE * punteroFile;
    punteroFile = fopen(DATA_FILE, "w+");

    //Si el fichero no existe o no se puede abrir.
    if( !punteroFile ){
        printf("Error abriendo el fichero %s\n", DATA_FILE);
        exit(EXIT_FAILURE);
    }

    //Se itera y escribe el contenido de la tabla hash en el fichero.
    for( int i = 0; i < tabla->size; i++ ) {
        //En la primera vuelta del bucle se inserta l aprimera fila de control del fichero
        if( i == 0){
            fprintf(punteroFile, "%s\n", "#nombre;id;matriculado;telefono;email;");
        }
        Ht_item * temp = tabla->items[i];
        while(temp != NULL){
            fprintf(punteroFile, "%s;%d;%s;%s;%s;\n", temp->nombre, *temp->id, temp->matriculado, temp->telefono, temp->email);
            temp = NULL;
        }
    }

    //Se cierra el puntero del File
    fclose(punteroFile);
}
 
/**
 * Se muestra por consola los elementos de la tabla.
 */
void listarTabla(HashTable* table) {
    printf("\n------------------------------------------------------\n");
    for (int i=0; i<table->size; i++) {
        if (table->items[i]) {
            printf("%d, ID:%d, Nombre:%s, Matriculado:%s, Teléfono:%s, Email:%s", i, *table->items[i]->id, table->items[i]->nombre, table->items[i]->matriculado, table->items[i]->telefono, table->items[i]->email);
            printf("\n");
        }
    }
    printf("------------------------------------------------------\n");
}

static void menuEditar() {
    
    char *result = NULL;
    int id;
    char matriculadoNuevo[3];
    
    printf("Menú editar alumno:\n\n");
    printf("1. Introduce el ID numérico, único, del alumno:\n");
    scanf("%d", &id);

    //Se busca si existe ese ID
    result = buscar(ht, &id);
    if( result != NULL ){
        printf("2. Introduce un nuevo valor sí/no para matriculado:\n");
        scanf("%s", matriculadoNuevo);

        if ( editarHt(ht, &id, matriculadoNuevo) == EXIT_SUCCESS )
        {
            printf("Alumno con ID:%d, matriculado:%s modificado correctamente\n", id, matriculadoNuevo);
            //Se guarda en el fichero los cambios
            guardaFile(ht);
        }
    } else {
        printf("Error: El ID:%d no existe para ningun alumno\n", id);
        return;
    }
}

/**
 * Función que a través de un menú por el prompot solicita al usuario los datos necesarios para la insercción de un alumno nuevo.
 */
static void menuInsertar() {
    
    char *result = NULL;
    int id;
    char nombreNuevo[80];
    char matriculadoNuevo[3];
    char telefonoNuevo[13];
    char emailNuevo[60];
    
    printf("Menú insertar alumno:\n\n");
    printf("1. Introduce un ID numérico, único, para el alumno:\n");
    scanf("%d", &id);

    //Se busca si existe ese ID
    result = buscar(ht, &id);
    if( result != NULL ){
        printf("Error: El ID:%d ya existe para el alumno: %s\n", id, result);
        return;
    } else {
        
        printf("2. Introduce un nombre del alumno:\n");
        scanf("%s", nombreNuevo);
        
        printf("3. Introduce un valor sí/no para matriculado:\n");
        scanf("%s", matriculadoNuevo);
        printf("4. Introduce el teléfono del alumno:\n");
        scanf("%s", telefonoNuevo);
        printf("5. Introduce el email del alumno:\n");
        scanf("%s", emailNuevo);
        
        if ( insertarItem(ht, &id, nombreNuevo, matriculadoNuevo, telefonoNuevo, emailNuevo) == EXIT_SUCCESS )
        {
            printf("Alumno con ID:%d, nombre:%s, matriculado:%s, teléfono:%s, email:%s, insertado correctamente\n", id, nombreNuevo, matriculadoNuevo, telefonoNuevo, emailNuevo);
            //Se guarda en el fichero los cambios
            guardaFile(ht);
        }
    }
}

/**
 * Fución que muestra el menú para borrar un alumno.
 */
static void menuBorrar() {
    int idBorrar;
    char *result = NULL;

    printf("Menú insertar alumno:\n\n");
    printf("1. Introduce un ID numérico, único, para el alumno a borrar:\n");
    scanf("%d", &idBorrar);

    //Se busca si existe ese ID
    result = buscar(ht, &idBorrar);
    if( result == NULL ){
        printf("Error: El ID:%d no existe para ningún alumno.\n", idBorrar);
        return;
    } else {
        // Se invoca a la función de borrar.
        if ( borrarAlumno(ht, &idBorrar) == EXIT_SUCCESS )
        {
            printf("Borrado con éxito\n");
            //Se guarda en el fichero los cambios
            guardaFile(ht);
        }
    }
    
}




/**
 * Función que muestra el menú principal con las distintas operaciones de la aplicación.
 */
void menu() {
    int choice;
    do
    {
     printf("\nMenú:\n\n");
     printf("1. Insertar registro de alumno\n");
     printf("2. Listar alumnos\n");
     printf("3. Modificar matriculación alumno\n");
     printf("4. Eliminar alumno\n");
     printf("0. Salir\n");

     scanf("%d",&choice);

     switch (choice)
     {
         case 1:
             menuInsertar();
             break;
         case 2:
             listarTabla(ht);
             break;
         case 3:
             menuEditar();
             break;
         case 4:
             menuBorrar();
             break;
         case 0:
             printf("¡Hasta pronto!\n");
             break;
         default:
             printf("Error: Opción incorrecta.\n");
             exit(EXIT_FAILURE);
     }
      
    } while (choice != 0);
}

/***
 *Función inicial que carga el fichero de datos y los almacena en una tabla hash en memoria.
 */
static void init() {
    
    FILE *filePointer = NULL;
    char linea[1024];

    //Se inicializa la tabla hash
    ht = create_table(CAPACITY);
    
    //Se carga el fichero de datos
    filePointer = fopen(DATA_FILE,"r");
    
    //Se comprueba si hay error al abrir
    if( filePointer == NULL){
        printf("Error abriendo el fichero %s\n", DATA_FILE);
        exit(EXIT_FAILURE);
    }

    int count = 0;
    while(fgets(linea, 1024, (FILE*) filePointer)) {
        //Nos saltamos la línea de control del fichero.
        if( count == 0 ){
            count++;
            continue;
        }
        //Se extraen las palabras separadas por ;
        char *token = strtok(linea, SEPARATOR);
        
        if(token != NULL){
            char *nombre = NULL;
            int *id = 0;
            char *matriculado = NULL;
            char *telefono = NULL;
            char *email = NULL;

            for(int i = 0; i <= 4; i++){
                switch (i) {
                    case 0:
                        nombre = token;
                        break;
                    case 1:
                        id = token;
                        break;
                    case 2:
                        matriculado = token;
                        break;
                    case 3:
                        telefono = token;
                        break;
                    case 4:
                        email = token;
                        break;
                    default:
                        break;
                }
                token = strtok(NULL, SEPARATOR);
            }
            //Insertamos los datos de la línea
            insertarItem(ht, &count, nombre, matriculado, telefono, email);
        }
        count++;
    }
     
    fclose(filePointer);
}

/**
 * Función principal.
 */
int main() {
    //Variables principales
    
    //Inicializar tabla hash a partir de los datos del fichero
    init();
    
    //Se carga el menú de opciones
    menu();
    
    //Se libera el espacio utilizado por la tabla hash.
    liberaTabla(ht);
    
    return 0;
}
