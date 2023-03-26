#include <bits/stdc++.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <essential.h>


int openSocket(int port);
pair<string, int> convertAddr(struct sockaddr_in addr);
struct sockaddr_in createAddr(string, int port);
void sendUDP(int socket, struct datagram request, struct sockaddr_in server_addr, socklen_t &server_addr_size) ;
struct datagram sendUDPrequest(int socket, struct datagram request, struct sockaddr_in server_addr, socklen_t &server_addr_size);
