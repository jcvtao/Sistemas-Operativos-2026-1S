#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 90
#define HASH_SIZE 1000000 // 1 millón de entradas = 8MB en RAM (dentro del límite de 10MB)

typedef struct {
    int id;
    char name[MAX_STR];
    char industry[MAX_STR];
    char country[MAX_STR];
    int employees;
    long next; // Offset al siguiente registro en caso de colisión de hash
} Company;

// Función Hash DJB2
unsigned long get_hash(unsigned char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash % HASH_SIZE;
}

// Función para parsear CSV respetando comillas
char* get_field(char **line) {
    char *start = *line;
    char *end;
    if (*start == '"') { // Campo con comillas
        start++;
        end = strchr(start, '"');
        if (end) {
            *end = '\0';
            *line = end + 2; // Saltar la coma después de la comilla
        }
    } else { // Campo normal
        end = strchr(start, ',');
        if (end) {
            *end = '\0';
            *line = end + 1;
        }
    }
    return start;
}

int main() {
    FILE *csv = fopen("../data/companies_sorted.csv", "r");
    FILE *data_bin = fopen("../bin/data.bin", "wb+");
    
    if (!csv || !data_bin) {
        printf("Error abriendo archivos.\n");
        return 1;
    }

    // Inicializar Tabla Hash en RAM (8MB aprox)
    // Guardamos el offset del primer elemento en data.bin para cada hash
    long *hash_table = malloc(HASH_SIZE * sizeof(long));
    for (int i = 0; i < HASH_SIZE; i++) hash_table[i] = -1;

    char line[2048];
    fgets(line, sizeof(line), csv); // Saltar cabecera

    int count = 0;
    while (fgets(line, sizeof(line), csv)) {
        char *ptr = line;
        Company c;

        if (ptr == NULL || *ptr == '\0') {
            continue;
        }
        
        // Parsear campos según tu dataset
        c.id = atoi(get_field(&ptr));
        strncpy(c.name, get_field(&ptr), MAX_STR);
        get_field(&ptr); // Saltar domain
        get_field(&ptr); // Saltar year founded
        strncpy(c.industry, get_field(&ptr), MAX_STR);
        get_field(&ptr); // Saltar size range
        get_field(&ptr); // Saltar locality
        strncpy(c.country, get_field(&ptr), MAX_STR);
        get_field(&ptr); // Saltar linkedin url
        c.employees = atoi(get_field(&ptr));
        
        unsigned long h = get_hash((unsigned char*)c.name);
        
        // Manejo de colisión: El nuevo registro apunta a lo que había antes en el hash
        c.next = hash_table[h];
        
        // Escribir en data.bin
        fseek(data_bin, 0, SEEK_END);
        long current_offset = ftell(data_bin);
        fwrite(&c, sizeof(Company), 1, data_bin);
        
        // Actualizar tabla hash con la posición del registro más reciente
        hash_table[h] = current_offset;

        if (++count % 1000000 == 0) {
            printf("Procesados %d registros...\n", count);
        }
    }

    // Guardar la tabla hash en un archivo para que el buscador la use
    FILE *idx_bin = fopen("../bin/index.bin", "wb");
    fwrite(hash_table, sizeof(long), HASH_SIZE, idx_bin);
    
    fclose(csv);
    fclose(data_bin);
    fclose(idx_bin);
    free(hash_table);
    
    printf("Indexación completada. %d registros procesados.\n", count);
    return 0;
}