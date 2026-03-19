//gcc sem_test.c -o sem_test -pthread
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>

#define NUM_HILOS 2

sem_t *sem;
#define MAX_HILOS 1

void *funcion_hilo(void *ap){
    // inicio sección crítica
    sem_wait(sem);
    printf("\nHilo: %i", *(int *)ap);
    sleep(2);
    printf("\nHilo: %i", *(int *)ap);
    fflush(stdout);
    //fin sección crítica
    sem_post(sem);
}

int main(){
  pthread_t id[NUM_HILOS];
  int *retval;
  int r, i, val[NUM_HILOS];

  sem = sem_open("semaforo_name", O_CREAT, 0700, MAX_HILOS);
  if(sem == SEM_FAILED){
    perror("Error al crear sem");
    exit(-1);
  }

  for(i = 0; i < NUM_HILOS; i++){
    val[i] = i;
    r = pthread_create( (pthread_t *)&id[i], NULL, funcion_hilo, (void *)&val[i]);
    if(r != 0){
        perror("Error en pthread_create");
        exit(-1);
    }
  }

  for(i = 0; i < NUM_HILOS; i++){
    r = pthread_join(id[i], (void **)&retval);
    if(r != 0){
        perror("Error en pthread_join");
        exit(-1);
    }
  }
  sem_close(sem);
	sem_unlink("semaforo_name");

  return 0;
}