%%writefile pipe_test.c

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

int main(){
    int pipefd[2], r;
    pid_t pid;
    float pi;

    r = pipe(pipefd);
    if(r == -1){
      perror("No se pudo crear la tubería: ");
      exit(-1);
    }
    pid = fork();
    if(pid == -1){
      perror("Error en el fork: ");
      exit(-1);
    }
    if(pid == 0){ //hijo
      pi = 3.14;
      close(pipefd[0]);
      r = write(pipefd[1], (void *)&pi, sizeof(pi));
      close(pipefd[1]);
    }else{        //padre
      close(pipefd[1]);
      read(pipefd[0], (void *)&pi, sizeof(float));
      close(pipefd[0]);
      printf("\nLlegó al padre: %f", pi);
    }
}
