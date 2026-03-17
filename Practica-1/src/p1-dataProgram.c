#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../include/structures.h"

// Dirección tuberías
#define REQ_FIFO "bin/req_fifo"
#define RES_FIFO "bin/res_fifo"

/**
 * @brief Limpia el buffer de entrada
 */
void clean_stdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/**
 * @brief Verifica que una cadena solo tenga letras
 */
int is_all_alpha(char *str) {
    for (int i = 0; str[i]; i++) 
        if (!isalpha(str[i]) && str[i] != ' ' && str[i] != ',') return 0;
    return 1;
}

int main() {
    int r;

    // Verifica que se haya indexado previamente
    FILE *f1 = fopen("bin/index_name.bin", "r");
    if (!f1) { 
        perror("Dataset has not been indexed. Please execute ./bin/indexer first"); 
        exit(-1); 
    }
    fclose(f1);

    // Creación de tuberías
    r = mkfifo(REQ_FIFO, 0666);
    if (r < 0) { 
        perror("Error mkfifo REQ_FIFO"); 
        exit(-1); 
    }

    r = mkfifo(RES_FIFO, 0666);
    if (r < 0) { 
        perror("Error mkfifo RES_FIFO"); 
        exit(-1); 
    }

    // Interfaz de usuario
    while (1) {
        printf("\n====== COMPANY SEARCH ======\n");
        printf("[1] Search by name\n[2] Search by country and industry\n[3] Exit\n");
        printf("============================\nSelect an option: ");
        
        char opt_s[10];
        scanf("%s", opt_s);
        if (!isdigit(opt_s[0])) {
            printf("Error: Enter a number.\n");
            continue;
        }
        int opt = atoi(opt_s);

        if (opt == 3) break; // SALIR

        SearchRequest req;
        req.type = opt;
        
        if (opt == 1) { // BÚSQUEDA POR NOMBRE
            printf("Enter name: "); clean_stdin();
            fgets(req.key1, MAX_STR, stdin);
            req.key1[strcspn(req.key1, "\n")] = 0;
            // Verificar entrada: nombre
            if (!is_all_alpha(req.key1)) {
                printf("Invalid name.\n");
                continue;
            }

        } else if (opt == 2) { // BÚSQUEDA POR PAÍS E INDUSTRIA
            printf("Enter country: "); clean_stdin();
            fgets(req.key1, MAX_STR, stdin);
            req.key1[strcspn(req.key1, "\n")] = 0;
            // Verificar entrada: país
            if (!is_all_alpha(req.key1)) {
                printf("Invalid country.\n");
                continue;
            }

            printf("Enter industry: ");
            fgets(req.key2, MAX_STR, stdin);
            req.key2[strcspn(req.key2, "\n")] = 0;
            // Verificar entrada: industria
            if (!is_all_alpha(req.key2)) {
                printf("Invalid industry.\n");
                continue;
            }
        }

        int fd_req = open(REQ_FIFO, O_WRONLY);
        if(fd_req < 0) { 
            perror("Error open req");
            continue;
        }
        write(fd_req, &req, sizeof(SearchRequest));
        close(fd_req);

        // Recibir resultados
        printf("\nResults found:\n");
        int fd_res = open(RES_FIFO, O_RDONLY);
        Company res;
        int encontrados = 0;
        while (read(fd_res, &res, sizeof(Company)) > 0) {
            if (res.name[0] == '\0') break; // Fin de resultados
            printf("- Name: %s | Country: %s | Industry: %s | Year: %s | Domain: %s\n", 
                    res.name, res.country, res.industry, res.year, res.domain);
            encontrados++;
        }
        close(fd_res);
        if (encontrados == 0) printf("No records found.\n");

        printf("\n============================\nWould you like to search again?\n");
        printf("[1] Yes\n[2] No (Exit)\nSelect an option: ");
        scanf("%s", opt_s);
        if (atoi(opt_s) == 2) break;
    }
    return 0;
}