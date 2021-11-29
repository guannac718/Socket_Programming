USC EE 450 Final Project

a. Name
  Guannan Chen
  
b. USC ID
  9635887510
  
c. 

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
    - 
