#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#define MAX 1000000000

double carga(){
  double x;
  int i;

  for(i = 0; i < MAX; i++){
    x = x + sin(i);
  }
}

int main()
{
  pid_t pid;

  pid = fork();
  if(pid == -1){
    perror("Error en fork: ");
    exit(-1);
  }

  if(pid == 0){
    printf("\nSoy el hijo...");
    carga();
  } else {
    printf("\nSoy el padre...");
    carga();
  }

}