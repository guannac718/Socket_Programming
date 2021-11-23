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
#include <cmath>
#include <set>
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

void findShortestPath(string name1, string name2, int curCost, string curPath, map<string, vector<string> >* graph, map<string, int> score, string* path, int* cost, set<string> set);

float cal_cost(int one, int two);

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
/*
void findShortestPath(string name1, string name2, int curCost, string curPath, map<string, vector<string> >* graph, map<string, int> score, string* path, int* cost, set<string> set) {
  if (name1 == name2) {
    cur += calc_cost(name1, name2, score);
    if (cur < cost) {
      cost = cur;
      path = curPath + " --- " + name2;
    }
    return;
  }
  //path += " --- " + name1;
  
  for (string nei : graph.at(name1)) {
    if (set.count(nei) == 0) {
      set.insert(nei);
      int c = calc_cost(name1, nei, score);
      curCost += c;
      string tmp = curPath;
      curPath += " --- " + nei;
      findShortestPath(nei, name2, curCost, curPath, graph, score, path, cost, set);
      curPath = tmp;
      curCost -= c;
      set.erase(nei);
    }
    
  }
}
*/

float cal_cost(int one, int two) {
  return (1.0 * abs(one - two)) / (1.0 * (one + two));
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

    
    for (map<string, int>::iterator it = scoreMap.begin(); it != scoreMap.end(); ++it) {
      cout << it->first << ":" << it->second << endl;
    }
    

    // Build graph 
    float adjmatrix[1000][1000];
    map<string, int> nameMap;
    map<int, string> indexMap;
    int index = 0;
    istringstream ssm(names);
    std::string relation;
    int edgeNum = 0;
    while (std::getline(ssm, relation, ',')) {
      cout << "relation is " << relation << endl;
      if (!relation.empty()) {
	size_t start;
	size_t end = 0;
	vector<string> nodes;
	string word = "";
	string prevWord = "";
	for (int i = 0; i < relation.length(); i++) {
	  if (relation[i] == ' ') {
	    if (nameMap.count(word) == 0) {
	      nameMap.insert(std::pair<string, int>(word, index));
	      indexMap.insert(std::pair<int, string>(index, word));
	      index++;
	    }
	    prevWord = word;
	    word = "";
	  } else if (i == relation.length() - 1) {
	    word = word + relation[i];
	    if (nameMap.count(word) == 0) {
	      nameMap.insert(std::pair<string, int>(word, index));
	      indexMap.insert(std::pair<int, string>(index, word));
	      index++;
	    }
	  } else {
	    word += relation[i];
	  }
	}
	cout << scoreMap.at(prevWord) << " ";
	cout << scoreMap.at(word) << endl;
	float cost = cal_cost(scoreMap.at(prevWord), scoreMap.at(word));
	cout << prevWord << " " << word << ":" << cost << endl;
	adjmatrix[nameMap.at(prevWord)][nameMap.at(word)] = cost;
	adjmatrix[nameMap.at(word)][nameMap.at(prevWord)] = cost;
      }
      edgeNum++;
     
      
    }

    cout << "nameMap: " << endl;
    for (map<string, int>::iterator it = nameMap.begin(); it != nameMap.end(); ++it) {
      cout << it->first << ":" << it->second << endl;
    }

    cout << "indexMap: " << endl;
    for (map<int, string>::iterator it = indexMap.begin(); it != indexMap.end(); ++it) {
      cout << it->first << ":" << it->second << endl;
    }
    
    for (int i = 0; i < edgeNum; i++) {
      for (int j = 0; j < edgeNum; j++) {
	cout << adjmatrix[i][j] << ' ';
      }
      cout << endl;
    }
    /*
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
