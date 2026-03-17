#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/structures.h"

unsigned long get_hash(unsigned char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) hash = ((hash << 5) + hash) + c;
    return hash % HASH_SIZE;
}

char* get_field(char **line) {
    if (*line == NULL || **line == '\0') return NULL;
    char *start = *line;
    if (*start == '"') {
        start++;
        char *end = strchr(start, '"');
        if (end) { *end = '\0'; *line = end + 2; }
    } else {
        char *end = strchr(start, ',');
        if (end) { *end = '\0'; *line = end + 1; }
        else { *line = NULL; }
    }
    return start;
}

int main() {
    int r;

    FILE *csv = fopen("data/companies_sorted.csv", "r");
    FILE *data_bin = fopen("bin/data.bin", "wb+");

    if (!csv || !data_bin) { 
        perror("Error en archivos"); 
        return 1; }

    printf("Indexando...\n");

    long *idx_name = malloc(HASH_SIZE * sizeof(long));
    long *idx_country_industry = malloc(HASH_SIZE * sizeof(long));

    if (!idx_name || !idx_country_industry) {
        perror("Error en malloc: ");
        exit(-1);
    }

    for(int i=0; i<HASH_SIZE; i++) idx_name[i] = idx_country_industry[i] = -1;

    char line[2048];
    fgets(line, sizeof(line), csv); // Cabecera

    int count = 0;
    while (fgets(line, sizeof(line), csv)) {
        char *ptr = line;
        Company c;
        
        get_field(&ptr);                                        // skip: #
        strncpy(c.name, get_field(&ptr) ? : "", MAX_STR);       // name
        strncpy(c.domain, get_field(&ptr) ? : "", MAX_STR);     // domain
        strncpy(c.year, get_field(&ptr) ? : "", 10);            // year
        strncpy(c.industry, get_field(&ptr) ? : "", MAX_STR);   // industry
        get_field(&ptr);                                        // skip: size range
        get_field(&ptr);                                        // skip: locality
        strncpy(c.country, get_field(&ptr) ? : "", MAX_STR);    // country
        // skip: linkedin url, current employee estimate, total employee estimate

        // Hash Nombre
        unsigned long h_name = get_hash((unsigned char*)c.name);
        c.next_name = idx_name[h_name];

        // Hash Pais + Industria
        char combined[MAX_STR*2];
        snprintf(combined, sizeof(combined), "%s%s", c.country, c.industry);
        unsigned long h_country_industry = get_hash((unsigned char*)combined);
        c.next_criteria = idx_country_industry[h_country_industry];

        fseek(data_bin, 0, SEEK_END);
        long offset = ftell(data_bin);
        r = fwrite(&c, sizeof(Company), 1, data_bin);
        if (r != 1) { 
            perror("Error fwrite"); 
            exit(-1); 
        }

        idx_name[h_name] = offset;
        idx_country_industry[h_country_industry] = offset;

        if (++count % 1000000 == 0) printf("Procesados %d...\n", count);
    }

    FILE *f1 = fopen("bin/index_name.bin", "wb");
    FILE *f2 = fopen("bin/index_country_industry.bin", "wb");

    if (!f1 || !f2) { 

        perror("Error en archivos de índice"); 
        exit(-1); 
    }

    r = fwrite(idx_name, sizeof(long), HASH_SIZE, f1);
    if (r != HASH_SIZE) { 
        perror("Error fwrite idx_name"); 
        exit(-1); 
    }

    fwrite(idx_country_industry, sizeof(long), HASH_SIZE, f2);
    if (r != HASH_SIZE) { 
        perror("Error fwrite idx_country_industry"); 
        exit(-1); 
    }

    fclose(csv); 
    fclose(data_bin); 
    fclose(f1); 
    fclose(f2);
    free(idx_name); 
    free(idx_country_industry);
    printf("Indexación terminada: %d registros.\n", count);
    return 0;
}