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
#define serverP_UDP_PORT 23510 // Server P port number
#define MAXDATASIZE 1024 // max number of bytes we can get at once
#define FAIL_CODE -1


/**
 * Defined global variables
 */
string namestr;
int sockfd_serverP;
struct sockaddr_in serverP_addr, central_addr;

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
  sockfd_serverP = socket(AF_INET, SOCK_DGRAM, 0); // Create a UDP socket
  if (sockfd_serverP == FAIL_CODE) {
      perror("[ERROR] serverP failed to create socket");
      exit(1);
  }
}

/**
 * Step 2: Create sockaddr_in struct
 */

void init_central_connection() {

    // Server A side information
    // Initialize server A IP address, port number
    memset(&serverP_addr, 0, sizeof(serverP_addr)); //  make sure the struct is empty
    serverP_addr.sin_family = AF_INET; // Use IPv4 address family
    serverP_addr.sin_addr.s_addr = inet_addr(LOCAL_HOST); // Host IP address
    serverP_addr.sin_port = htons(serverP_UDP_PORT); // Server A port number
}


/**
 * Step 3: Bind socket with specified IP address and port number
 */
void bind_socket() {
  if (::bind(sockfd_serverP, (struct sockaddr *) &serverP_addr, sizeof(serverP_addr)) == FAIL_CODE) {
    
    perror("[ERROR] ServerP failed to bind Central server's UDP socket");
    exit(1);
  }
    

    printf("The Server P is up and running using UDP on port %d. \n", serverP_UDP_PORT);
}


int main() {

  create_socket();
    
  init_central_connection();
   
  bind_socket();

  
  while (true) {

    // Receive data from Central server
    socklen_t central_addr_size = sizeof(central_addr);
    if (::recvfrom(sockfd_serverP, rec_buffer, sizeof(rec_buffer), 0, (struct sockaddr *) &central_addr, &central_addr_size) == FAIL_CODE) {
      perror("[ERROR] ServerP failed to receive data from Central server");
      exit(1);
    }

    //cout << "rec_buffer is " << rec_buffer << endl;
    printf("ServerT received a request from Central to get the topology \n");

    // Split received data into graph and scores
    vector<string> namesvec;
    string inputstr(rec_buffer);
    istringstream iss(inputstr);
    std::string token;
    while (std::getline(iss, token, '.')) {
      if (!token.empty()) {
	namesvec.push_back(token);
      }
    }
    string names = namesvec[0];
    string scores = namesvec[1];
    scores = scores.substr(0, scores.length() - 1);
    cout << "names are " << names << endl;
    //cout << "scores are " << scores << endl;
    
    // Build scores map
    map<string, int> scoreMap;
    std::string scorepair;
    istringstream sss(scores);
    while (std::getline(sss, scorepair, ',')) {
      //cout << "scorepair is " << scorepair << endl;
      string word = "";
      string name = "";
      if (!scorepair.empty()) {
	for (int i = 0; i < scorepair.length(); i++) {
	  if (scorepair[i] == ' ') {
	    name = word;
	    word = "";
	    //cout << "name is " << name << endl;
	  } else if (i == scorepair.length() - 1) {
	    word += scorepair[i];
	    //cout << "score is " << word << endl;
	    int tmp = std::stoi(word);
	    //cout << "name and score is "  << name << word << endl;
	    scoreMap.insert(std::pair<string, int>(name, tmp));
	  } else {
	    word = word + scorepair[i];
	  }
	}
      }
      
    }

    /*
    for (map<string, int>::iterator it = scoreMap.begin(); it != scoreMap.end(); ++it) {
      cout << it->first << ":" << it->second << endl;
    }
    */

    // Build graph with structure map<string, vector<string> >
    map<string, vector<string> > graph;
    istringstream ssm(names);
    std::string relation;
    while (std::getline(ssm, relation, ',')) {
      cout << "relation is " << relation << endl;
      relation = relation.substr(0, relation.length() - 1);
      string cur = "";
      string node = "";
      vector<string> neis;
      if (!relation.empty()) {
	for (int i = 0; i < relation.length(); i++) {
	  if (relation[i] == ' ') {
	    if (node == "") {
	      node = cur;
	    } else {
	      neis.push_back(cur);
	      
	    }
	    cur = "";
	    //cout << "node is " << node << endl;
	  } else if (i == relation.length() - 1) {
	    cur += relation[i];
	    neis.push_back(cur);
	    cout << "neis are ";
	    for (string nei : neis) {
	      cout << nei;
	    }
	    cout << endl;
	    //scoreMap.insert(std::pair<string, int>(name, tmp));
	  } else {
	    cur = cur + relation[i];
	  }
	}
      }
    }
    /*
    // Read graph from edgelist.txt
    std::ifstream graphInput("edgelist.txt");
    if (graphInput == NULL) {
      perror("[ERROR] ServerT: edgelist.txt not found.");
      exit(1);
    }
    map<string, int> nameMap;
    map<int, string> indexMap;
    int adjmatrix[1000][1000];
    int index = 0;
    while (graphInput.eof() != true) {
      std::string line;
      std::getline(graphInput, line);
      //cout << line << endl;
      if (!line.empty()) {
	 size_t start;
	 size_t end= 0;
	 vector<string> nodes;
      
	 string word = "";
	 string prevWord = "";
	 for (int i = 0; i < line.length(); i++) {
	   if (line[i] == ' ') {
	     //nodes.push_back(word);
	     //cout << word << endl;
	     if (nameMap.count(word) == 0) {
	       nameMap.insert(std::pair<string, int>(word, index));
	       indexMap.insert(std::pair<int, string>(index, word));
	       index++;
	     }
	     prevWord = word;
	     word = "";
	   } else if (i == line.length() - 1) {
	     word = word + line[i];
	     //cout << word << endl;
	     if (nameMap.count(word) == 0) {
	       nameMap.insert(std::pair<string, int>(word, index));
	       indexMap.insert(std::pair<int, string>(index, word));
	       index++;
	     }	    
	   } else {
	     word = word + line[i];
	   }
	 }
	 //cout << nameMap.at(prevWord) << endl;
	 //cout << nameMap.at(word) << endl;
	 adjmatrix[nameMap.at(prevWord)][nameMap.at(word)] = 1;
	 adjmatrix[nameMap.at(word)][nameMap.at(prevWord)] = 1;
      }
      // delete &line;
     
      
    }
    */
    
    
    //cout << "Finished building map." << endl;

    /*
    for (map<string, int>::iterator it = nameMap.begin(); it != nameMap.end(); it++) {
      cout << it->first << " => " << it->second << endl;
    }
    cout << "name1's index is " << nameMap.at(name1) << endl;
    cout << "name2's index is " << nameMap.at(name2) << endl;
    */
    /*
    int index_1 = nameMap.at(name1);
    int index_2 = nameMap.at(name2);
    int size = nameMap.size();
    vector<string> graph;
    list<int> indices;
    string nei = "";
    nei += name1 + " ";
    for (int j = 0; j < size; j++) {
      if (adjmatrix[index_1][j] == 1) {
	indices.push_back(j);
	nei += indexMap.at(j) + " ";
      }
    }
    //cout << nei << endl;
    nei = nei.substr(0, nei.length());
    graph.push_back(nei);
    nei = "";
    for (list<int>::iterator ind = indices.begin(); ind != indices.end(); ind++) {
      nei += indexMap.at(*ind) + " ";
      for (int j = 0; j < size; j++) {
	if (j != index_1 && adjmatrix[*ind][j] == 1) {
	  nei += indexMap.at(j) + " ";
	}	
      }
      //cout << nei << endl;
      nei = nei.substr(0, nei.length());
      graph.push_back(nei);
      nei = "";
    }

    string tmp = "";
    for (std::vector<string>::iterator it = graph.begin(); it != graph.end(); it++) {
      tmp += *it;
      if (it != graph.end() - 1) {
	tmp += ",";
      }
      
    }
    char tmp_char[MAXDATASIZE];
    strcpy(tmp_char, tmp.c_str());
    cout << tmp_char << endl;
    
    if (sendto(sockfd_serverT, tmp_char, sizeof(tmp_char), 0, (struct sockaddr *) &central_addr, sizeof(central_addr)) == FAIL_CODE) {
      perror("[ERROR] ServerT failed to send data to Central Server.");
      exit(1);
    }
    */
  }
  
  close(sockfd_serverP);
  return 0; 
}
