#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

#define REQ_FIFO "bin/req_fifo"
#define RES_FIFO "bin/res_fifo"
#define DATA_BIN "bin/data.bin"

/**
 * @brief Crea tubería
 */
void crear_pipe(const char *path) {
    if (mkfifo(path, 0666) == -1) {
        // Errores distintos a una tubería ya existente
        if (errno != EEXIST) {
            perror("Error crítico al crear tubería");
            exit(1);
        }
    }
}

int main() {
    printf("Starting execution...\n");

    // Verificar si existe data.bin
    if (access(DATA_BIN, F_OK) == -1) {
        if (system("./bin/indexer") != 0) { // Ejecutar indexer 
            fprintf(stderr, "Error executing indexer.\n");
            return 1;
        }
    }

    // Crear las tuberías de comunicación
    printf("Creating pipes...\n");
    crear_pipe(REQ_FIFO);
    crear_pipe(RES_FIFO);

    // Ejecutar el buscador en una terminal nueva
    printf("Starting searcher...\n");
    system("gnome-terminal --title=\"SEARCHER\" -- bash -c \"./bin/searcher; exec bash\" &");
    sleep(1);

    // Ejecutar la interfaz en una terminal nueva
    printf("Starting menu...\n");
    system("gnome-terminal --title=\"MENU\" -- bash -c \"./bin/menu; exec bash\" &");

    printf("Everything set, you can start searching companies!\n");

    return 0;
}