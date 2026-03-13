#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>


int main(){
  key_t llave = 1234;
  int shm_id, r;
  size_t size = sizeof(double);
  double pi = 3.141592;
  double *ap;
  pid_t pid;

  shm_id = shmget(llave, size , IPC_CREAT | 0666);
  if(shm_id == -1){
      perror("Error al crear memoria compartida: ");
      exit(-1);
  }

  ap = (double *)shmat(shm_id, 0, 0);
  if(ap == -1){
      perror("Error en shmat");
      exit(-1);
  }

  pid = fork();
  if(pid == -1){
    perror("Error en el fork: ");
    exit(-1);
  }
  if(pid == 0){ //hijo
    *ap = pi;
  }else{        //padre
    wait(NULL);
    printf("%f", *ap);
  }

  r = shmdt(ap);
  if(r == (void *)(-1)){
      perror("Error en shmdt");
  }
  return 0;    
}