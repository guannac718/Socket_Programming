#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>

using namespace std;

#define LOCAL_HOST "127.0.0.1"
#define Central_clientA_TCP_PORT 25510
#define Central_clientB_TCP_PORT 26510
#define FAIL -1
#define BACKLOG 10
#define MAXDATASIZE 1024

int sockfd_clientA_TCP;
int sockfd_clientB_TCP;
int child_sockfd_clientA;
int child_sockfd_clientB;
struct sockaddr_in central_clientA_addr, central_clientB_addr;
struct sockaddr_in dest_clientA_addr, dest_clientB_addr;
string nameA;
string nameB;
char input_buf[MAXDATASIZE];
char inputB_buf[MAXDATASIZE];

void create_clientA_TCP_socket();

void create_clientB_TCP_socket();

void listen_clientA();

void listen_clientB();

void create_clientA_TCP_socket() {
  sockfd_clientA_TCP = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd_clientA_TCP == FAIL) {
    perror("[ERROR] Central: fail to create socket for client A");
    exit(1);
  }
  
  memset(&central_clientA_addr, 0, sizeof(central_clientA_addr));
  central_clientA_addr.sin_family = AF_INET;
  central_clientA_addr.sin_addr.s_addr = inet_addr(LOCAL_HOST);
  central_clientA_addr.sin_port = htons(Central_clientA_TCP_PORT);

  if (::bind(sockfd_clientA_TCP, (struct sockaddr *) &central_clientA_addr, sizeof(central_clientA_addr)) == FAIL) {
    perror("[ERROR] Central: fail to bind client A socket");
    exit(1);
  }
}

void create_clientB_TCP_socket() {
  sockfd_clientB_TCP = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd_clientB_TCP == FAIL) {
    perror("[ERROR] Central: fail to create socket for client B");
    exit(1);
  }
  
  memset(&central_clientB_addr, 0, sizeof(central_clientB_addr));
  central_clientB_addr.sin_family = AF_INET;
  central_clientB_addr.sin_addr.s_addr = inet_addr(LOCAL_HOST);
  central_clientB_addr.sin_port = htons(Central_clientB_TCP_PORT);

  if (::bind(sockfd_clientB_TCP, (struct sockaddr *) &central_clientB_addr, sizeof(central_clientB_addr)) == FAIL) {
    perror("[ERROR] Central: fail to bind client A socket");
    exit(1);
  }
}

void listen_clientA() {
  if (listen(sockfd_clientA_TCP, BACKLOG) == FAIL) {
    perror("[ERROR] Central: fail to listen for client A socket");
    exit(1);
  }
}

void listen_clientB() {
  if (listen(sockfd_clientB_TCP, BACKLOG) == FAIL) {
    perror("[ERROR] Central: fail to listen for client B socket");
    exit(1);
  }
}

int main() {

  create_clientA_TCP_socket();
  create_clientB_TCP_socket();
  listen_clientA();
  listen_clientB();
  printf("The central server is up and running \n");


  while (1) {
    socklen_t clientA_addr_size = sizeof(dest_clientA_addr);
    child_sockfd_clientA = ::accept(sockfd_clientA_TCP, (struct sockaddr *) &dest_clientA_addr, &clientA_addr_size);
    if (child_sockfd_clientA == FAIL) {
      perror("[ERROR] Central server failed to accept connection with client A.");
      exit(1);
    }
    int recA = recv(child_sockfd_clientA, input_buf, MAXDATASIZE, 0);
    if (recA == FAIL) {
      perror("[ERROR] Central server failed to receive data from client A.");
      exit(1);
    }
   	
    char data_buffer[MAXDATASIZE];
    strncpy(data_buffer, input_buf, strlen(input_buf));
    nameA = strtok(input_buf, " ");
    printf("The Central server received input=\"%s\" from the client using TCP over port %d \n", nameA.c_str(), Central_clientA_TCP_PORT);


    socklen_t clientB_addr_size = sizeof(dest_clientB_addr);
    child_sockfd_clientB = ::accept(sockfd_clientB_TCP, (struct sockaddr *) &dest_clientB_addr, &clientB_addr_size);
    if (child_sockfd_clientB == FAIL) {
      perror("[ERROR] Central server failed to accept connection with client B.");
      exit(1);
    }
    int recB = recv(child_sockfd_clientB, inputB_buf, MAXDATASIZE, 0);
    if (recB == FAIL) {
      perror("[ERROR] Central server failed to receive data from client B.");
      exit(1);
    }
   	
    char dataB_buffer[MAXDATASIZE];
    strncpy(dataB_buffer, inputB_buf, strlen(inputB_buf));
    nameB = strtok(inputB_buf, " ");
    printf("The Central server received input=\"%s\" from the client using TCP over port %d \n", nameB.c_str(), Central_clientB_TCP_PORT);
  }
  

  close(sockfd_clientA_TCP);
  close(sockfd_clientB_TCP);
  return 0;
}
