#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

// Definiciones idénticas al indexer y menu
#define MAX_STR 90
#define HASH_SIZE 500000 
#define REQ_FIFO "bin/req_fifo"
#define RES_FIFO "bin/res_fifo"

typedef struct {
    char name[MAX_STR];
    char domain[MAX_STR];
    char year[10];
    char industry[MAX_STR];
    char locality[MAX_STR];
    char country[MAX_STR];
    char linkedin[120];
    long next_name;     
    long next_criteria; 
} Company;

typedef struct {
    int type; // 1: Nombre, 2: Pais+Industria, 3: Salir
    char key1[MAX_STR]; 
    char key2[MAX_STR]; 
} SearchRequest;

// Función Hash DJB2 (Debe ser idéntica en todos los archivos)
unsigned long get_hash(unsigned char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash % HASH_SIZE;
}

int main() {
    // 1. Reservar y cargar índices en RAM (Total 8MB)
    long *idx_name = malloc(HASH_SIZE * sizeof(long));
    long *idx_country_industry = malloc(HASH_SIZE * sizeof(long));
    
    if (!idx_name || !idx_country_industry) {
        perror("Error al asignar memoria para índices");
        return 1;
    }

    FILE *f_n = fopen("bin/index_name.bin", "rb");
    FILE *f_c = fopen("bin/index_country_industry.bin", "rb");
    if (!f_n || !f_c) {
        perror("Error: No se encontraron los archivos de índice. Ejecuta el indexer primero");
        return 1;
    }

    fread(idx_name, sizeof(long), HASH_SIZE, f_n);
    fread(idx_country_industry, sizeof(long), HASH_SIZE, f_c);
    fclose(f_n); fclose(f_c);

    // 2. Abrir el archivo de datos principal
    FILE *data_f = fopen("bin/data.bin", "rb");
    if (!data_f) {
        perror("Error al abrir data.bin");
        return 1;
    }

    printf("[SEARCHER] Servicio activo y cargado en RAM (8MB). Esperando peticiones...\n");

    SearchRequest req;
    while (1) {
        // Abrir FIFO de peticiones (se bloquea hasta que el Menú envíe algo)
        int fd_req = open(REQ_FIFO, O_RDONLY);
        if (fd_req < 0) { perror("Error abriendo req_fifo"); sleep(1); continue; }

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
                    fseek(data_f, offset, SEEK_SET);
                    Company c;
                    fread(&c, sizeof(Company), 1, data_f);

                    // Comparación exacta (ignorando el hash por colisiones)
                    if (strcasecmp(c.name, req.key1) == 0) {
                        write(fd_res, &c, sizeof(Company));
                        encontrados++;
                    }
                    offset = c.next_name; // Seguir la cadena en el archivo
                }
            } 
            else if (req.type == 2) { // BÚSQUEDA POR PAÍS E INDUSTRIA
                printf("[LOG] Buscando por Pais: '%s' e Industria: '%s'\n", req.key1, req.key2);
                
                // Re-crear la llave combinada como se hizo en el indexer
                char combined[MAX_STR * 2];
                snprintf(combined, sizeof(combined), "%s%s", req.key1, req.key2);
                
                h = get_hash((unsigned char*)combined);
                offset = idx_country_industry[h];

                while (offset != -1) {
                    fseek(data_f, offset, SEEK_SET);
                    Company c;
                    fread(&c, sizeof(Company), 1, data_f);

                    // Verificar ambos criterios para confirmar el match
                    if (strcasecmp(c.country, req.key1) == 0 && strcasecmp(c.industry, req.key2) == 0) {
                        write(fd_res, &c, sizeof(Company));
                        encontrados++;
                    }
                    offset = c.next_criteria;
                }
            }

            printf("[LOG] Búsqueda finalizada. %d resultados enviados.\n", encontrados);

            // Enviar "sentinela" (estructura vacía) para avisar al Menú que terminamos
            Company sentinel;
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