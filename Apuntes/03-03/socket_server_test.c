#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
//#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>

#define PORT    3535
#define BACKLOG 8

int main(){

  int sockfd, r;
  struct sockaddr_in server, cliente;
  socklen_t addrlen, addrlen_c;
  double pi;
  // int sockfdc1;

  // crear socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd == -1){
    perror("Error en socket");
    exit(-1);
  }

  server.sin_family = AF_INET;    //ipv4
  server.sin_port = htons(PORT);  //endianismo
  server.sin_addr.s_addr = INADDR_ANY;
  bzero(&(server.sin_zero), 8);
  addrlen = sizeof(struct sockaddr);
  //nombrar socket
  r = bind(sockfd, (struct sockaddr *)&server, addrlen);
  if(r == -1){
    perror("Error en bind");
    exit(-1);
  }

  r = listen(sockfd, BACKLOG);
  if(r == -1){
    perror("Error en listen");
    exit(-1);
  }

  sockfdc1 = accept(sockfd, (struct sockaddr *)&cliente, &addrlen_c);
  if(sockfdc1 == -1){
    perror("Error en accept");
    exit(-1);
  }

  pi = 3.14159265;
  r = send(sockfdc1, (void *)&pi, sizeof(pi), 0);
  if(r == -1){
    perror("error en send");
  }
  // WARNING: send envía bytes, el programador debe verificar que se hayan enviado todos los bytes!!!

  close(sockfdc1);
  close(sockfd);

}
