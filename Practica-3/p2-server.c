#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "structures.h"

#define PORT 8080
#define MAX_CLIENTS 32

// Descriptores de los índices en disco (heredados por fork, sin malloc)
int fd_name, fd_ci;
volatile sig_atomic_t n_clients = 0;

/**
 * @brief Recoge procesos hijo terminados para evitar zombies
 */
void handle_sigchld(int sig) {
    (void)sig;
    while (waitpid(-1, NULL, WNOHANG) > 0) n_clients--;
}

/**
 * @brief Función Hash DJB2
 */
unsigned long get_hash(unsigned char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) hash = ((hash << 5) + hash) + c;
    return hash % HASH_SIZE;
}

/**
 * @brief Escribe una entrada en el log con el formato requerido:
 *        [Fecha YYYYMMDDTHHMMSS] Cliente [IP] [búsqueda - origen - destino]
 */
void write_log(char *ip, int type, char *k1, char *k2) {
    FILE *log = fopen("search-history.log", "a");
    if (!log) { perror("Error abriendo log"); return; }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char ts[20];
    strftime(ts, sizeof(ts), "%Y%m%dT%H%M%S", t);

    if (type == 1)
        fprintf(log, "[Fecha %s] Cliente [%s] [nombre - %s - ]\n", ts, ip, k1);
    else
        fprintf(log, "[Fecha %s] Cliente [%s] [pais_industria - %s - %s]\n", ts, ip, k1, k2);

    fclose(log);
}

/**
 * @brief Proceso hijo: atiende todas las peticiones de un cliente.
 *
 *        Uso de memoria dinámica:
 *        - malloc()  : reserva el buffer inicial de resultados
 *        - realloc() : duplica la capacidad cuando se llena
 *        - free()    : libera el buffer al terminar cada búsqueda
 *
 *        Los índices permanecen en disco; pread() lee únicamente
 *        el offset del cubeto hash (8 bytes) sin mover el cursor
 *        del archivo, lo que es seguro con múltiples procesos hijo.
 */
void attend_client(int client_fd, char *client_ip) {
    int r;

    FILE *data_f = fopen("bin/data.bin", "rb");
    if (!data_f) {
        perror("Error abriendo data.bin");
        close(client_fd);
        exit(-1);
    }

    SearchRequest req;
    while (read(client_fd, &req, sizeof(SearchRequest)) > 0) {
        write_log(client_ip, req.type, req.key1, req.key2);

        // Reservar buffer dinámico para acumular resultados
        int encontrados = 0;
        int capacidad = 8;
        Company *resultados = malloc(capacidad * sizeof(Company));
        if (!resultados) { perror("Error en malloc"); break; }

        unsigned long h;
        long offset;

        if (req.type == 1) { // BÚSQUEDA POR NOMBRE
            printf("[LOG] Buscando por nombre: '%s'\n", req.key1);
            h = get_hash((unsigned char*)req.key1);
            pread(fd_name, &offset, sizeof(long), h * sizeof(long));

            while (offset != -1) {
                Company c;
                r = fseek(data_f, offset, SEEK_SET);
                if (r != 0) { perror("Error buscando empresa(s)"); break; }
                r = fread(&c, sizeof(Company), 1, data_f);
                if (r <= 0) { perror("Error leyendo empresa"); break; }

                if (strcasecmp(c.name, req.key1) == 0) {
                    // Ampliar buffer si se llenó
                    if (encontrados >= capacidad) {
                        capacidad *= 2;
                        Company *tmp = realloc(resultados, capacidad * sizeof(Company));
                        if (!tmp) { perror("Error en realloc"); break; }
                        resultados = tmp;
                    }
                    resultados[encontrados++] = c;
                }
                offset = c.next_name;
            }

        } else if (req.type == 2) { // BÚSQUEDA POR PAÍS E INDUSTRIA
            printf("[LOG] Buscando por Pais: '%s' e Industria: '%s'\n", req.key1, req.key2);

            // Llave combinada con memoria dinámica
            char *combined = malloc(MAX_STR * 2);
            if (!combined) { perror("Error en malloc"); free(resultados); break; }
            snprintf(combined, MAX_STR * 2, "%s%s", req.key1, req.key2);
            h = get_hash((unsigned char*)combined);
            free(combined);

            pread(fd_ci, &offset, sizeof(long), h * sizeof(long));

            while (offset != -1) {
                Company c;
                r = fseek(data_f, offset, SEEK_SET);
                if (r != 0) { perror("Error buscando empresa(s)"); break; }
                r = fread(&c, sizeof(Company), 1, data_f);
                if (r <= 0) { perror("Error leyendo empresa"); break; }

                if (strcasecmp(c.country, req.key1) == 0 && strcasecmp(c.industry, req.key2) == 0) {
                    if (encontrados >= capacidad) {
                        capacidad *= 2;
                        Company *tmp = realloc(resultados, capacidad * sizeof(Company));
                        if (!tmp) { perror("Error en realloc"); break; }
                        resultados = tmp;
                    }
                    resultados[encontrados++] = c;
                }
                offset = c.next_criteria;
            }
        }

        printf("[LOG] Búsqueda finalizada. %d resultados enviados.\n", encontrados);

        // Enviar todos los resultados de una vez y liberar el buffer
        if (encontrados > 0)
            write(client_fd, resultados, encontrados * sizeof(Company));
        free(resultados);

        Company sentinel; // Empresa vacía para finalizar
        memset(&sentinel, 0, sizeof(Company));
        write(client_fd, &sentinel, sizeof(Company));
    }

    fclose(data_f);
    close(client_fd);
    exit(0);
}

int main() {
    // Abrir índices en disco (sin cargar en RAM, sin malloc)
    fd_name = open("bin/index_name.bin", O_RDONLY);
    fd_ci   = open("bin/index_country_industry.bin", O_RDONLY);
    if (fd_name < 0 || fd_ci < 0) {
        perror("Error: ¡No se encontraron los archivos de índice! Ejecuta ./bin/indexer primero");
        exit(-1);
    }

    // Registrar manejador SIGCHLD para evitar procesos zombie
    struct sigaction sa;
    sa.sa_handler = handle_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa, NULL);

    // Crear socket del servidor
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("Error creando socket"); exit(-1); }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) { perror("Error en bind"); exit(-1); }
    if (listen(server_fd, MAX_CLIENTS) < 0) { perror("Error en listen"); exit(-1); }

    printf("[SERVIDOR] Servicio activo en puerto %d. Esperando clientes...\n", PORT);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) { perror("Error en accept"); continue; }

        // Rechazar si se alcanzó el límite de clientes simultáneos
        if (n_clients >= MAX_CLIENTS) {
            printf("[AVISO] Límite de %d clientes alcanzado. Rechazando conexión.\n", MAX_CLIENTS);
            close(client_fd);
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        printf("[LOG] Nueva conexión de %s\n", client_ip);

        pid_t pid = fork();
        if (pid < 0) {
            perror("Error en fork");
            close(client_fd);
        } else if (pid == 0) { // Proceso hijo: atiende al cliente
            close(server_fd);
            attend_client(client_fd, client_ip);
        } else { // Proceso padre: sigue escuchando
            close(client_fd);
            n_clients++;
        }
    }

    close(fd_name); close(fd_ci);
    close(server_fd);
    return 0;
}