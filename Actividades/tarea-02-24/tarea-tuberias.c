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
    int descriptores_tuberia[2];
    pid_t id_proceso;
    double parte_padre, parte_hijo, valor_pi_total;
    long long punto_medio = MAX / 2;

    if (pipe(descriptores_tuberia) == -1) {
        perror("Error al crear la tubería");
        exit(-1);
    }

    id_proceso = fork();

    if (id_proceso == -1) {
        perror("Error en el fork");
        exit(-1);
    }

    if (id_proceso == 0) { // HIJO
        close(descriptores_tuberia[0]);
        parte_hijo = calcular_pi(0, punto_medio);
        write(descriptores_tuberia[1], &parte_hijo, sizeof(double));
        close(descriptores_tuberia[1]);
        exit(0);
    } else { // PADRE
        close(descriptores_tuberia[1]);
        parte_padre = calcular_pi(punto_medio, MAX);
        
        read(descriptores_tuberia[0], &parte_hijo, sizeof(double));
        close(descriptores_tuberia[0]);

        valor_pi_total = parte_padre + parte_hijo;
        printf("====== Metodo: Tuberia ======\n");
        printf("Resultado con %lld iteraciones: %.10f\n", MAX, valor_pi_total);
    }

    return 0;
}