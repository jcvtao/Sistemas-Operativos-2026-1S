/* Sistemas Operativos Parcial 1 (2026-1S)
 * Integrantes:
 * - Yadixon David Alfonso Chiquillo - yalfonsoc@unal.edu.co
 * - Federico Puentes Acosta - fpuentesa@unal.edu.co
 * - Juan Camilo Vergara Tao - juvergarat@unal.edu.co 
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) { // Ejecución sin indicar archivo a vigilar
        printf("Se debe ejecutar como: %s <archivo_a_vigilar>\n", argv[0]);
        printf("e.g. %s archivo-prueba.txt\n", argv[0]);
        exit(-1);
    }

    char *filename = argv[1];
    struct stat file_stat;
    int s, r;
    
    s = stat(filename, &file_stat); // Estado inicial del archivo
    if (s == -1) {
        perror("Error al acceder al archivo");
        exit(-1);
    }

    time_t last_mod_time = file_stat.st_mtime;
    printf("=====================================\n"
           "Vigilando archivo: %s\nPresiona Ctrl+C para salir\n"
           "=====================================\n", filename);

    while (1) {
        sleep(2);
        s = stat(filename, &file_stat);
        if (s != 0) {
            perror("Error al acceder al archivo"); // El archivo desapareció o cambió de nombre
            exit(-1);
        }

        // Comparar fecha de modificación
        if (file_stat.st_mtime != last_mod_time) {
            printf("El archivo tuvo un cambio. Realizando copia de seguridad...\n");
            
            // Creación de backup
            char command[256];
            snprintf(command, sizeof(command), "cp %s ./data/backup/%s.bak", filename, filename);

            r = system(command); // Ejecución de comando
            if (r == -1) {
                perror("Error crítico al ejecutar comando utilizando system");
                exit(-1);
            } else if (r != 0) {
                perror("Error al ejecutar comando cp (copy). Asegúrate de que el directorio './data/backup' exista");
                exit(-1);
            }
            
            // Actualizar último tiempo de modificación
            last_mod_time = file_stat.st_mtime;
            printf("Archivo copiado con éxito en ./data/backup/%s.bak\n\n", filename);
        }
    }

    return 0;
}