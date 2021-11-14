#include <vector>
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
#include <string>
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <fstream>

using namespace std;

/**
 * serverA.cpp
 * A storage server, possesses a database file database.txt
 * in which attribute values regarding information of links
 * are stored
 *
*/

/**
 * Named Constants
 */
#define LOCAL_HOST "127.0.0.1" // Host address
#define serverT_UDP_PORT 21510 // Server A port number
#define MAXDATASIZE 1024 // max number of bytes we can get at once
#define FAIL_CODE -1


/**
 * Defined global variables
 */
string namestr;
int sockfd_serverT;
struct sockaddr_in serverT_addr, central_addr;

fstream database;
int link_num = 0;

char rec_buffer[MAXDATASIZE]; 
char result[MAXDATASIZE]; 



/**
 * Defined functions
 */

// 1. Create UDP socket
void create_socket();

// 2. Initialize connection with AWS server
void init_central_connection();

// 3. Bind a socket
void bind_socket();

// 4. Receive data from AWS

// 5. Write the data into database (database.txt)

// 6. Search request data and send result to AWS server

/**
 * Step 1: Create server A UDP sockect
 */
void create_socket() {
  sockfd_serverT = socket(AF_INET, SOCK_DGRAM, 0); // Create a UDP socket
  if (sockfd_serverT == FAIL_CODE) {
      perror("[ERROR] serverT failed to create socket");
      exit(1);
  }
}

/**
 * Step 2: Create sockaddr_in struct
 */

void init_central_connection() {

    // Server A side information
    // Initialize server A IP address, port number
  memset(&serverT_addr, 0, sizeof(serverT_addr)); //  make sure the struct is empty
    serverT_addr.sin_family = AF_INET; // Use IPv4 address family
    serverT_addr.sin_addr.s_addr = inet_addr(LOCAL_HOST); // Host IP address
    serverT_addr.sin_port = htons(serverT_UDP_PORT); // Server A port number
}


/**
 * Step 3: Bind socket with specified IP address and port number
 */
void bind_socket() {
  if (::bind(sockfd_serverT, (struct sockaddr *) &serverT_addr, sizeof(serverT_addr)) == FAIL_CODE) {
    
    perror("[ERROR] ServerT failed to bind Central server's UDP socket");
    exit(1);
  }
    

    printf("The Server T is up and running using UDP on port %d. \n", serverT_UDP_PORT);
}

int main() {

  create_socket();
    
  init_central_connection();
   
  bind_socket();

  
  while (true) {
    
    socklen_t central_addr_size = sizeof(central_addr);
    if (::recvfrom(sockfd_serverT, rec_buffer, sizeof(rec_buffer), 0, (struct sockaddr *) &central_addr, &central_addr_size) == FAIL_CODE) {
      perror("[ERROR] ServerT failed to receive data from Central server");
      exit(1);
    }

    char name_buffer[MAXDATASIZE];
    strncpy(name_buffer, rec_buffer, strlen(rec_buffer));
    namestr = strtok(rec_buffer, " ");
    printf("ServerT received a request from Central to get the topology \n");
    cout << name_buffer << endl;
    cout << namestr << endl;
    
    vector<string> names;
    std:: string input(name_buffer);
    size_t start;
    size_t end= 0;
    while ((start = namestr.find_first_not_of(",", end)) != string::npos) {
      end = namestr.find(",", start);
      names.push_back(namestr.substr(start, end - start));
    }
    cout << names[0] << endl;
    cout << names[1] << endl;
    
  }
  
  close(sockfd_serverT);
  return 0; 
}
