#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>

#define PORT    3535

int main(){

  int fd, r;
  double pi;
  struct sockaddr_in server;
  socklen_t addrlen;

  fd = socket(AF_INET, SOCK_STREAM, 0);
  if(r == -1){
    perror("Error en bind");
    exit(-1);
  }

  server.sin_family = AF_INET;    //ipv4
  server.sin_port = htons(PORT);  //endianismo
  server.sin_addr.s_addr = inet_addr("127.0.0.1"); //cadena de dir ip a ip enteros
  bzero(&(server.sin_zero), 8);
  addrlen = sizeof(struct sockaddr);
  r = connect(fd, (struct sockaddr *)&server, addrlen);
  if(r == -1){
      perror("Error en connect");
      exit(-1);
  }

  r = recv(fd, (void *)&pi, sizeof(double), 0);
  if(r == -1){
      perror("Error en recv");
      exit(-1);
  }
  printf("Mensaje del servidor: %f", pi);
  close(fd);

}
