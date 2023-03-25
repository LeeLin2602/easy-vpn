#pragma once
#include <bits/stdc++.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

enum action_type {
    request_ip,
    offer_ip
};

struct datagram {
    char room[64];
    action_type action;
    int length;
    char payload[16384];
};

int openSocket(int port);
struct sockaddr_in createAddr(char *ip, int port);
inline void sendUDP(int socket, struct datagram request, struct sockaddr_in server_addr, socklen_t &server_addr_size) ;

struct datagram sendUDPrequest(int socket, struct datagram request, struct sockaddr_in server_addr, socklen_t &server_addr_size);
