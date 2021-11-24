#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sstream>
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
#define Central_Client_TCP_PORT 25510 // AWS port number
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
char final_res[MAXDATASIZE]; // Store input to compute (send to AWS)
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
    perror("[ERROR] client: cannot open client socket ");
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
  cout << endl;
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
    perror("[ERROR] client A failed to send data.");
  }
  printf("The client sent %s to the Central server \n", write_buf);

  
  if (recv(sockfd_client_TCP, final_res, sizeof(final_res), 0) == FAIL) {
    perror("[ERROR] Client A failed to receive result from the Central server.");
    exit(1);
  }

  //cout << "Final res is " << final_res << endl;
  string res(final_res);
  if (final_res[0] == '!') {
    res = res.substr(1, sizeof(res));
    vector<string> epath;
    size_t estart;
    size_t eend = 0;
    while ((estart = res.find_first_not_of(",", eend)) != string::npos) {
      eend = res.find(",", estart);
      string ename = res.substr(estart, eend - estart);
      epath.push_back(ename);
    }
    cout << "Found no compatibility for " << epath[0] << " and " << epath[1] << endl;
  } else {
    string names = "";
  string cost = "";
  for (int i = 0; i < res.length(); i++) {
    if (res[i] == ',') {
      names = cost;
      cost = "";
    } else if (i == res.length() - 1) {
      cost = cost + res[i];
    } else {
      cost += res[i];
    }
  }

  //cout << "names are " << names.c_str() << endl;
  //cout << "cost is " << cost.c_str() << endl;

  vector<string> path;
  size_t start;
  size_t end = 0;
  while ((start = names.find_first_not_of(" ", end)) != string::npos) {
    end = names.find(" ", start);
    string name = names.substr(start, end - start);
    path.push_back(name);
  }

  float gap;
  stringstream ssc(cost);
  ssc >> gap;
  

  string name1 = path[0];
  string name2 = path[path.size() - 1];

  cout << "Found compatibility for " << name1.c_str() << " and " << name2.c_str() << ":" << endl;
  for (int i = 0; i < path.size(); i++) {
    cout << path[i].c_str();
    if (i != path.size() - 1) {
      cout << " --- ";
    }
  }
  cout << endl;
  printf("Compatibility score: %.2f \n", gap);
  }
  
   




  
  close(sockfd_client_TCP);
  return 0;
}
