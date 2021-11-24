#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <list>
#include <string.h>
#include <netdb.h>
#include <map>
#include <climits>
#include <sys/types.h>
#include <string>
#include <netinet/in.h>
#include <cmath>
#include <set>
#include <sys/socket.h>
#include <algorithm>
#include <climits>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <fstream>

using namespace std;

#define LOCAL_HOST "127.0.0.1" // Host address
#define serverP_UDP_PORT 23510 // Server P port number
#define MAXDATASIZE 50000 // max number of bytes we can get at once
#define FAIL_CODE -1

string namestr;
int sockfd_serverP;
struct sockaddr_in serverP_addr, central_addr;

fstream database;
int verNum = 0;
string res = "";
map<string, int> nameMap;
map<int, string> indexMap;
char rec_buffer[MAXDATASIZE]; 
char result[MAXDATASIZE];
bool seenA;
bool seenB;


void create_socket();

void init_central_connection();

void bind_socket();

float cal_cost(int one, int two);

void dijkstra(vector<vector<float> > graph, int src, int dest);

int minDistance(float dist[], bool sptSet[]);

void printPath(int parent[], int j);

void printSolution(float dist[], int n, int parent[], int dest);


void create_socket() {
  sockfd_serverP = socket(AF_INET, SOCK_DGRAM, 0); 
  if (sockfd_serverP == FAIL_CODE) {
      perror("[ERROR] serverP failed to create socket");
      exit(1);
  }
}

void init_central_connection() {

    memset(&serverP_addr, 0, sizeof(serverP_addr));
    serverP_addr.sin_family = AF_INET; 
    serverP_addr.sin_addr.s_addr = inet_addr(LOCAL_HOST);
    serverP_addr.sin_port = htons(serverP_UDP_PORT);
}


void bind_socket() {
  if (::bind(sockfd_serverP, (struct sockaddr *) &serverP_addr, sizeof(serverP_addr)) == FAIL_CODE) {
    
    perror("[ERROR] ServerP failed to bind Central server's UDP socket");
    exit(1);
  }
    

    printf("The ServerP is up and running using UDP on port %d. \n", serverP_UDP_PORT);
}

float cal_cost(int one, int two) {
  return (1.0 * abs(one - two)) / (1.0 * (one + two));
}

// GeeksForGeeks Dijkstra's algorithm with min path
void dijkstra(vector<vector<float> > graph, int src, int dest) {
  float dist[verNum];
  bool sptSet[verNum];
  int parent[verNum];
  for (int i = 0; i < verNum; i++) {
    parent[0] = -1;
    dist[i] = INT_MAX;
    sptSet[i] = false;
  }

  dist[src] = 0;
  for (int count = 0; count < verNum - 1; count++) {
    int u = minDistance(dist, sptSet);
    sptSet[u] = true;
    for (int v = 0; v < verNum; v++) {
      if (!sptSet[v] && graph[u][v] && dist[u] + graph[u][v] > 0 && dist[u] + graph[u][v] < INT_MAX && dist[u] + graph[u][v] < dist[v]) {
	parent[v] = u;
	dist[v] = dist[u] + graph[u][v];
      }
    }
  }
  printSolution(dist, verNum, parent, dest);
}

int minDistance(float dist[], bool sptSet[]) {
  int min = INT_MAX;
  int min_index;
  for (int v = 0; v < verNum; v++) {
    if (sptSet[v] == false && dist[v] <= min) {
      min = dist[v];
      min_index = v;
    }
  }
  return min_index;
}

void printPath(int parent[], int j) {
  if (parent[j] == -1) {
    return;
  }
  printPath(parent, parent[j]);
  res += indexMap.at(j) + " ";
}

void printSolution(float dist[], int n, int parent[], int dest) {
  int src = 0;
  for (int i = 1; i < 2; i++) {
    res += indexMap.at(src);
    res += " ";
    printPath(parent, dest);
       
  }
  string cost;
  stringstream ssc;
  ssc << dist[dest];
  cost = ssc.str();
  res += "," + cost;
}


int main() {


  setvbuf(stdout, NULL, _IONBF, 0);
  create_socket();
    
  init_central_connection();
   
  bind_socket();

  
  while (true) {

     memset(rec_buffer, 0, sizeof(rec_buffer));
     memset(result, 0, sizeof(result));
     res = "";
     verNum = 0;
     nameMap.clear();
     indexMap.clear();
     seenA = false;
     seenB = false;

    // Receive data from Central server
    socklen_t central_addr_size = sizeof(central_addr);
    if (::recvfrom(sockfd_serverP, rec_buffer, sizeof(rec_buffer), 0, (struct sockaddr *) &central_addr, &central_addr_size) == FAIL_CODE) {
      perror("[ERROR] ServerP failed to receive data from Central server");
      exit(1);
    }

    printf("The ServerP received the topology and score information. \n");

    // Split received data into graph and scores
    vector<string> namesvec;
    string inputstr(rec_buffer);
    istringstream iss(inputstr);
    std::string token;
    while (std::getline(iss, token, ';')) {
      if (!token.empty()) {
	namesvec.push_back(token);
      }
    }
    string inputs = namesvec[0];
    string name1;
    string name2;
    for (int i = 0; i < inputs.length(); i++) {
	if (inputs[i] == ',') {
	  name1 = name2;
          name2 = "";
	} else if (i == inputs.length() - 1) {
          name2 += inputs[i];
	} else {
          name2 = name2 + inputs[i];
	}
    }
    string remaining = namesvec[1];
    vector<string> inputvec;
    istringstream tmp(remaining);
    std::string curBuffer;
    while (std::getline(tmp, curBuffer, '.')) {
      if (!curBuffer.empty()) {
	inputvec.push_back(curBuffer);
      }
    }
    string names = inputvec[0];
    string scores = inputvec[1];
    scores = scores.substr(0, scores.length() - 1);

    
    // Build scores map
    map<string, int> scoreMap;
    std::string scorepair;
    istringstream sss(scores);
    while (std::getline(sss, scorepair, ',')) {

      string word = "";
      string name = "";
      if (!scorepair.empty()) {
	for (int i = 0; i < scorepair.length(); i++) {
	  if (scorepair[i] == ' ') {
	    name = word;
	    word = "";

	  } else if (i == scorepair.length() - 1) {
	    word += scorepair[i];

	    int tmp;
	    stringstream ssword(word);
	    ssword >> tmp;

	    scoreMap.insert(std::pair<string, int>(name, tmp));
	  } else {
	    word = word + scorepair[i];
	  }
	}
      }
      
    }
    seenA = scoreMap.count(name1) == 1;
    seenB = scoreMap.count(name2) == 1;
    if (!seenA || !seenB) {
       char error_char[MAXDATASIZE];
       string e = "";
       strcpy(error_char, e.c_str());
    
       if (sendto(sockfd_serverP, error_char, sizeof(error_char), 0, (struct sockaddr *) &central_addr, sizeof(central_addr)) == FAIL_CODE) {
	 perror("[ERROR] ServerP failed to send data to Central Server.");
	 exit(1);
       }
    }
    else {

       // Build graph 
    float adjmatrix[1000][1000];
    int index = 0;
    istringstream ssm(names);
    std::string relation;
    while (std::getline(ssm, relation, ',')) {

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

	float cost = cal_cost(scoreMap.at(prevWord), scoreMap.at(word));

	adjmatrix[nameMap.at(prevWord)][nameMap.at(word)] = cost;
	adjmatrix[nameMap.at(word)][nameMap.at(prevWord)] = cost;
      }
      verNum++;   
    }


    vector<vector<float> > graph;
    for (int i = 0; i < verNum; i++) {
      vector<float> cur;
      for (int j = 0; j < verNum; j++) {
	cur.push_back(adjmatrix[i][j]);
      }
  
      graph.push_back(cur);
    }
    
 

    dijkstra(graph, nameMap.at(name1), nameMap.at(name2));

    
    char tmp_char[MAXDATASIZE];
    strcpy(tmp_char, res.c_str());
    cout << "ready to send " << tmp_char << endl;
    
    if (sendto(sockfd_serverP, tmp_char, sizeof(tmp_char), 0, (struct sockaddr *) &central_addr, sizeof(central_addr)) == FAIL_CODE) {
      perror("[ERROR] ServerP failed to send data to Central Server.");
      exit(1);
    }
    printf("The ServerP finished sending the results to the Central. \n");
    }
  }
  
  close(sockfd_serverP);
  return 0; 
}
