#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "../include/structures.h"

// Dirección tuberías
#define REQ_FIFO "bin/req_fifo"
#define RES_FIFO "bin/res_fifo"

/**
 * @brief Función Hash DJB2
 */
unsigned long get_hash(unsigned char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash % HASH_SIZE;
}

int main() {
    int r;

    // Reservar y cargar índices en RAM
    long *idx_name = malloc(HASH_SIZE * sizeof(long));
    long *idx_country_industry = malloc(HASH_SIZE * sizeof(long));
    
    if (!idx_name || !idx_country_industry) {
        perror("Error al asignar memoria para índices");
        exit(-1);
    }

    FILE *f_n = fopen("bin/index_name.bin", "rb");
    FILE *f_c = fopen("bin/index_country_industry.bin", "rb");
    if (!f_n || !f_c) {
        perror("Error: ¡No se encontraron los archivos de índice! Ejecuta ./bin/indexer primero");
        exit(-1);
    }

    fread(idx_name, sizeof(long), HASH_SIZE, f_n);
    fread(idx_country_industry, sizeof(long), HASH_SIZE, f_c);
    fclose(f_n); fclose(f_c);

    // Abrir data.bin
    FILE *data_f = fopen("bin/data.bin", "rb");
    if (!data_f) {
        perror("Error al abrir data.bin");
        exit(-1);
    }

    printf("[SEARCHER] Servicio activo. Esperando peticiones...\n");

    SearchRequest req;
    while (1) {
        // Abrir FIFO de peticiones
        int fd_req = open(REQ_FIFO, O_RDONLY);
        if (fd_req < 0) {
            perror("[ERROR] Error abriendo req_fifo. Ejecuta ./bin/p1-dataProgram primero");
            sleep(3);
            continue;
        }

        if (read(fd_req, &req, sizeof(SearchRequest)) > 0) {
            // Abrir FIFO de respuesta
            int fd_res = open(RES_FIFO, O_WRONLY);
            if (fd_res < 0) { perror("Error abriendo res_fifo"); close(fd_req); continue; }

            unsigned long h;
            long offset;
            int encontrados = 0;

            if (req.type == 1) { // BÚSQUEDA POR NOMBRE
                printf("[LOG] Buscando por nombre: '%s'\n", req.key1);
                h = get_hash((unsigned char*)req.key1);
                offset = idx_name[h];

                while (offset != -1) {
                    r = fseek(data_f, offset, SEEK_SET); // Buscar colisiones
                    if (r != 0) {
                        perror("Error buscando empresa(s)");
                    }

                    Company c;
                    r = fread(&c, sizeof(Company), 1, data_f);
                    if (r <= 0) {
                        perror("Error leyendo empresa");
                    }
                    
                    // Comparación de nombre (verificación)
                    if (strcasecmp(c.name, req.key1) == 0) {
                        write(fd_res, &c, sizeof(Company));
                        encontrados++;
                    }
                    offset = c.next_name;
                }
            } 
            else if (req.type == 2) { // BÚSQUEDA POR PAÍS E INDUSTRIA
                printf("[LOG] Buscando por Pais: '%s' e Industria: '%s'\n", req.key1, req.key2);
                
                // Llave combinada
                char combined[MAX_STR * 2];
                snprintf(combined, sizeof(combined), "%s%s", req.key1, req.key2);
                
                h = get_hash((unsigned char*)combined);
                offset = idx_country_industry[h];

                while (offset != -1) {
                    r = fseek(data_f, offset, SEEK_SET);
                    if (r != 0) {
                        perror("Error buscando empresa(s)");
                    }

                    Company c;
                    r = fread(&c, sizeof(Company), 1, data_f);
                    if (r <= 0) {
                        perror("Error leyendo empresa");
                    }

                    // Comparación de país e inudstria (verificación)
                    if (strcasecmp(c.country, req.key1) == 0 && strcasecmp(c.industry, req.key2) == 0) {
                        write(fd_res, &c, sizeof(Company));
                        encontrados++;
                    }
                    offset = c.next_criteria;
                }
            }

            printf("[LOG] Búsqueda finalizada. %d resultados enviados.\n", encontrados);

            Company sentinel; // Empresa vacía para finalizar
            memset(&sentinel, 0, sizeof(Company));
            write(fd_res, &sentinel, sizeof(Company));
            
            close(fd_res);
        }
        close(fd_req);
    }

    free(idx_name);
    free(idx_country_industry);
    fclose(data_f);
    return 0;
}