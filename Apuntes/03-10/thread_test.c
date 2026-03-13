#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void *funcion_hilo(void *ap){
    printf("Hilo: %f", *(double *)ap);
}

int main(){
  double pi = 3.141592;
  pthread_t id;
  int *retval;
  int r;

  r = pthread_create( (pthread_t *)&id, NULL, funcion_hilo, (void *)&pi);
  if(r != 0){
      perror("Error en pthread_create");
      exit(-1);
  }

  r = pthread_join(id, (void **)&retval);
  if(r != 0){
      perror("Error en pthread_join");
      exit(-1);
  }
  return 0;
}