#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../include/structures.h"

#define SERVER_IP "127.0.0.1"
#define PORT 8080

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
    // Conectar al servidor
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) { perror("Error creando socket"); exit(-1); }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &addr.sin_addr);

    if (connect(sock_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Error conectando al servidor. ¿Está en ejecución p2-server?");
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
            if (!is_all_alpha(req.key1)) {
                printf("Invalid name.\n");
                continue;
            }

        } else if (opt == 2) { // BÚSQUEDA POR PAÍS E INDUSTRIA
            printf("Enter country: "); clean_stdin();
            fgets(req.key1, MAX_STR, stdin);
            req.key1[strcspn(req.key1, "\n")] = 0;
            if (!is_all_alpha(req.key1)) {
                printf("Invalid country.\n");
                continue;
            }

            printf("Enter industry: ");
            fgets(req.key2, MAX_STR, stdin);
            req.key2[strcspn(req.key2, "\n")] = 0;
            if (!is_all_alpha(req.key2)) {
                printf("Invalid industry.\n");
                continue;
            }
        } else {
            printf("Invalid option.\n");
            continue;
        }

        write(sock_fd, &req, sizeof(SearchRequest));

        // Recibir y acumular resultados en buffer dinámico
        int encontrados = 0;
        int capacidad = 8;
        Company *resultados = malloc(capacidad * sizeof(Company));
        if (!resultados) { perror("Error en malloc"); break; }

        Company res;
        while (read(sock_fd, &res, sizeof(Company)) > 0) {
            if (res.name[0] == '\0') break; // Fin de resultados

            if (encontrados >= capacidad) {
                capacidad *= 2;
                Company *tmp = realloc(resultados, capacidad * sizeof(Company));
                if (!tmp) { perror("Error en realloc"); break; }
                resultados = tmp;
            }
            resultados[encontrados++] = res;
        }

        // Mostrar resultados y liberar buffer
        printf("\nResults found:\n");
        for (int i = 0; i < encontrados; i++)
            printf("- Name: %s | Country: %s | Industry: %s | Year: %s | Domain: %s\n",
                    resultados[i].name, resultados[i].country, resultados[i].industry,
                    resultados[i].year, resultados[i].domain);
        free(resultados);

        if (encontrados == 0) printf("No records found.\n");

        printf("\nPress any key to continue...");
        clean_stdin(); getchar();
    }

    close(sock_fd);
    return 0;
}
