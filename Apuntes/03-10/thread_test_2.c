#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_HILOS 8

void *funcion_hilo(void *ap){

    printf("\nHilo: %i", *(int *)ap);
}

int main(){
  double pi = 3.141592;
  pthread_t id[NUM_HILOS];
  int *retval;
  int r, i, val[NUM_HILOS];

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
  return 0;
}