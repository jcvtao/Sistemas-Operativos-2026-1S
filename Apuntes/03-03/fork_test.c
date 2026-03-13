#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

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
  } else {
    printf("\nSoy el padre...");
  }

}