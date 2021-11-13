#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

/**
 * Client.cpp
 * A client, sends write and compute data to AWS server
 *
*/


/**
 * Named Constants
 */
#define LOCAL_HOST "127.0.0.1" // Host address
#define Central_Client_TCP_PORT 26510 // AWS port number
#define MAXDATASIZE 1024 // max number of bytes we can get at once
#define FAIL -1 // socket fails if result = -1


/**
 * Defined global variables
 */

string name; // Operation to do
//string bandwidth, length, velocity, noise_power; // Data to write
//string link_id, size, signal_power; // Data to compute
//double t_tran, t_prop, end_to_end;


int sockfd_client_TCP; // Client socket
struct sockaddr_in central_addr; // Central server address

char write_buf[MAXDATASIZE]; // Store input to write (send to AWS)
//char compute_buf[MAXDATASIZE]; // Store input to compute (send to AWS)
char write_result[MAXDATASIZE]; // Write result from AWS
//char compute_result[MAXDATASIZE]; // Compute result from AWS

/**
 * Steps (defined functions):
 */

// 1. Create TCP socket
void create_client_socket_TCP();

// 2. Initialize TCP connection with AWS
void init_central_server_connection();

// 3. Send connection request to AWS server
void request_central_server_connection();



//Function implementations
// Create TCP socket
void create_client_socket_TCP() {
  sockfd_client_TCP = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd_client_TCP == FAIL) {
    perror("[ERROR] client B: cannot open client socket ");
    exit(1);
  }
}

// Init TCP connection
void init_central_server_connection() {
  memset(&central_addr, 0, sizeof(central_addr));
  central_addr.sin_family = AF_INET;
  central_addr.sin_addr.s_addr = inet_addr(LOCAL_HOST);
  central_addr.sin_port = htons(Central_Client_TCP_PORT);
}

void request_central_server_connection() {
  connect(sockfd_client_TCP, (struct sockaddr *) &central_addr, sizeof(central_addr));

  printf("The client is up and running \n");
}

int main(int argc, char *argv[]) {

  create_client_socket_TCP();
  init_central_server_connection();
  request_central_server_connection();

  string input = argv[1];
  // printf("The input name is %s \n", input.c_str());
  strncpy(write_buf, input.c_str(), MAXDATASIZE);
  //printf("The write buffer is %s \n", write_buf);
  if(send(sockfd_client_TCP, write_buf, sizeof(write_buf), 0) == FAIL) {
    perror("[ERROR] client B failed to send data.");
  }
  printf("The client sent %s to the Central server \n", write_buf);

  close(sockfd_client_TCP);
  return 0;
}
