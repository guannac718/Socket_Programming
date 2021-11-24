#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <list>
#include <string.h>
#include <netdb.h>
#include <map>
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
 * Named Constants
 */
#define LOCAL_HOST "127.0.0.1" // Host address
#define serverS_UDP_PORT 22510 // Server S port number
#define MAXDATASIZE 50000 // max number of bytes we can get at once
#define FAIL_CODE -1


/**
 * Defined global variables
 */
string namestr;
int sockfd_serverS;
struct sockaddr_in serverS_addr, central_addr;

fstream database;
int link_num = 0;





/**
 * Defined functions
 */


void create_socket();


void init_central_connection();


void bind_socket();

void create_socket() {
  sockfd_serverS = socket(AF_INET, SOCK_DGRAM, 0); // Create a UDP socket
  if (sockfd_serverS == FAIL_CODE) {
      perror("[ERROR] serverS failed to create socket");
      exit(1);
  }
}



void init_central_connection() {


    memset(&serverS_addr, 0, sizeof(serverS_addr)); 
    serverS_addr.sin_family = AF_INET; 
    serverS_addr.sin_addr.s_addr = inet_addr(LOCAL_HOST); 
    serverS_addr.sin_port = htons(serverS_UDP_PORT); 
}


void bind_socket() {
  if (::bind(sockfd_serverS, (struct sockaddr *) &serverS_addr, sizeof(serverS_addr)) == FAIL_CODE) {
    
    perror("[ERROR] ServerS failed to bind Central server's UDP socket");
    exit(1);
  }
    

    printf("The ServerS is up and running using UDP on port %d. \n", serverS_UDP_PORT);
}


int main() {

  

  create_socket();
    
  init_central_connection();
   
  bind_socket();

  
  while (true) {

    char rec_buffer[MAXDATASIZE]; 
    char result[MAXDATASIZE]; 

    // Receive data from Central server
    socklen_t central_addr_size = sizeof(central_addr);
    if (::recvfrom(sockfd_serverS, rec_buffer, sizeof(rec_buffer), 0, (struct sockaddr *) &central_addr, &central_addr_size) == FAIL_CODE) {
      perror("[ERROR] ServerS failed to receive data from Central server");
      exit(1);
    }

    char name_buffer[MAXDATASIZE];

    printf("The ServerS received a request from Central to get the scores. \n");

    // Read graph from score.txt
    std::ifstream scoreInput("scores.txt");
    if (scoreInput == NULL) {
      perror("[ERROR] ServerS: scores.txt not found.");
      exit(1);
    }
    map<string, string> scoreMap;
    while (scoreInput.eof() != true) {
      std::string line;
      std::getline(scoreInput, line);
      if (!line.empty()) {
	 size_t start;
	 size_t end= 0;
      
	 string word = "";
	 string name = "";
	 string score = "";
	 for (int i = 0; i < line.length(); i++) {
	   if (line[i] == ' ') {
	   
	     name = word;
	     word = "";
	   } else if (i == line.length() - 1) {
	     word = word + line[i];
	     if (scoreMap.count(word) == 0) {
	       scoreMap.insert(std::pair<string, string>(name, word));
	     }	    
	   } else {
	     word = word + line[i];
	   }
	 }

      }
      
     
      
    }

    std:: string input(rec_buffer);
    size_t start;
    size_t end= 0;
    string res = "";
    while ((start = input.find_first_not_of(",", end)) != string::npos) {
      end = input.find(",", start);
      string newinput = input.substr(start, end - start);
      size_t newstart;
      size_t newend= 0;
      while ((newstart = newinput.find_first_not_of(" ", newend)) != string::npos) {
	newend = newinput.find(" ", newstart);
	string tmp = newinput.substr(newstart, newend - newstart);
        res += tmp + " ";
	res += scoreMap.at(tmp) + ",";
      }
    
    }


    // Send back to Central server
    char tmp_char[MAXDATASIZE];
    strcpy(tmp_char, res.c_str());

    
    if (sendto(sockfd_serverS, tmp_char, sizeof(tmp_char), 0, (struct sockaddr *) &central_addr, sizeof(central_addr)) == FAIL_CODE) {
      perror("[ERROR] ServerS failed to send data to Central Server.");
      exit(1);
    }
    printf("The ServerS finished sending the scores to Central. \n");
    
   
  }
  
  close(sockfd_serverS);
  return 0; 
}
