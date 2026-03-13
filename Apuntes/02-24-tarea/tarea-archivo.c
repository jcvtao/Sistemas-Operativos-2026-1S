#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#define MAX 1000000000LL

double calcular_pi(long long inicio, long long fin) {
    double suma_acumulada = 0.0;
    for (long long i = inicio; i < fin; i++) {
        if (i % 2 == 0) suma_acumulada += 1.0 / (2 * i + 1);
        else suma_acumulada -= 1.0 / (2 * i + 1);
    }
    return suma_acumulada * 4.0;
}

int main() {
    pid_t id_proceso;
    double parte_padre, parte_hijo, valor_pi_total;
    FILE *archivo_comunicacion;
    long long punto_medio = MAX / 2;

    id_proceso = fork();

    if (id_proceso == -1) {
        perror("Error en el fork");
        exit(-1);
    }

    if (id_proceso == 0) { // HIJO
        parte_hijo = calcular_pi(0, punto_medio);
        archivo_comunicacion = fopen("datos_pi.dat", "wb");
        if (archivo_comunicacion != NULL) {
            fwrite(&parte_hijo, sizeof(double), 1, archivo_comunicacion);
            fclose(archivo_comunicacion);
        }
        exit(0);
    } else { // PADRE
        parte_padre = calcular_pi(punto_medio, MAX);

        wait(NULL); // Sincronización

        archivo_comunicacion = fopen("datos_pi.dat", "rb");
        if (archivo_comunicacion != NULL) {
            fread(&parte_hijo, sizeof(double), 1, archivo_comunicacion);
            fclose(archivo_comunicacion);
        }

        valor_pi_total = parte_padre + parte_hijo;
        printf("====== Metodo: Archivo ======\n");
        printf("Resultado con %lld iteraciones: %.10f\n", MAX, valor_pi_total);
    }

    return 0;
}