USC EE 450 Final Project

a. Name
  Guannan Chen
  
b. USC ID
  9635887510
  
c. 
  I have implemented a model where the Central server is able to take two input names from two clients with one input for each client, and based on a given graph     and a score list, calculate the minimum matching gap and find out the path that is associated with the min gap. During this process, the Central server has to       communicate with Server T where the graph is obtained, serverS where the score is provided, and serverP where the final calculation is being conducted. After       getting the result, the Central server will return the result to the clients. Also, the program will detect whether the two names are able to be paired, since       there exists the possibility that no path could be found to connect those two people.
d. Code files
  1. clientA.cpp
    - Responsible for taking an input
    - Create TCP socket with Central server
    - Connect with Central server
    - Send name that is to be matched to Central server
    - Receive matching result from the Central server 
  2. clientB.cpp
    - Responsible for taking an input
    - Create TCP socket with Central server
    - Connect with Central server
    - Send name that is to be matched to Central server
    - Receive matching result from the Central server 
  3. central.cpp
    - Responsible for taking inputs from client, obtaining information from backend servers and forwarding the result to the client
    - Create TCP sockets with clients
    - Create UDP socket for communicating with backend servers
    - Forward data entries to serverT to obtain topology
    - Forward data entries to serverS to obtain scores
    - Send information to serverP to process and receive result
    - Forward result to clients
  4. serverT.cpp
    - Store topology from "edgelist.txt"
    - Receive data from Central server and send back topology
  5. serverS.cpp
    - Store scores from "scores.txt"
    - Receive data from Central server and send back scores
  6. serverP.cpp
    - Receive topology and scores from Central server
    - Calculate matching gap and send back to Central server
    
e. Message transfer
  When a server/client sends or receivs data, I put everything together in a string sperated by space, and then store them in a char array in order to send over TCP    and UDP. When the receiver has the message, I convert the char array into a string and split the string into several parts using the special character that          sperates each other.
  
g. Idiosyncrasy 
  Based on the test results on a given Virtual Machine(Ubuntu), there was no idiosyncrasy found.
  
f. Citations
  1. Beej's Socket Programming: https://www.beej.us/guide/bgnet/
    Create sockets for TCP & UDP
    Bind a socket
    Send and receive data
  2. Geeksforgeeks Dijkstra's algorithm with minimum path: https://www.geeksforgeeks.org/printing-paths-dijkstras-shortest-path-algorithm/
    Find the minimum matching gap as well as the path associated with.
