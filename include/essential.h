#pragma once
#include <thread>
#include <crypto.h>
#include <netconf.h>
#include <netio.h>
#include <server.h>

enum action_type {
    create_room,
    request_ip,
    offer_ip,
	forward_traffic,
    success,
    error
};

struct datagram {
    char room[64];
    action_type action;
    int length;
    /* unsigned ip; */
    char payload[16384];
	char checksum[65];
};

string hashDatagram(struct datagram &data);

/* void sniffer(int tun, int socket, struct sockaddr_in server_addr, char *room_name, char *room_pswd); */
void launchClient(char *server_ip, int service_port, char *room_name, char *room_pswd, char *ip_pool, int action = 0);
void launchServer(int service_port);
