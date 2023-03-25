#include <bits/stdc++.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <essential.h>


int openSocket(int port);
struct sockaddr_in createAddr(char *ip, int port);
inline void sendUDP(int socket, struct datagram request, struct sockaddr_in server_addr, socklen_t &server_addr_size) ;
struct datagram sendUDPrequest(int socket, struct datagram request, struct sockaddr_in server_addr, socklen_t &server_addr_size);
