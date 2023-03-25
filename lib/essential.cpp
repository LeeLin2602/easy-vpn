#include <essential.h>

void sniffer(int tun){
    const int buf_size = 1024 * 64;
    char buf[buf_size];

    while(1) {
        int n = read(tun, buf, buf_size);
        cout << n << ": " << buf << endl;
    }
}

void createRoom(struct sockaddr_in server_addr, char *room_name, char *room_pswd, char *ip_pool) {
    int socket = openSocket(-1);

    struct sockaddr_in serv_addr = server_addr;
    socklen_t serv_addr_size = sizeof(serv_addr);
    struct datagram request, reply;
    
    string message = room_pswd;
    message += ',';
    message += ip_pool;

    request.action = create_room;
    strcpy(request.room, room_name);
    request.length = message.size();
    strcpy(request.payload, (char *)message.c_str());

    reply = sendUDPrequest(socket, request, serv_addr, serv_addr_size);
    cout << reply.action << ": " << reply.payload << endl;
    if(reply.action == error) exit(1);
}

string requestIP(struct sockaddr_in server_addr, char *room_name, char *room_pswd) {
    int socket = openSocket(-1);

    struct sockaddr_in serv_addr = server_addr;
    socklen_t serv_addr_size = sizeof(serv_addr);
    struct datagram request, reply;
    
    request.action = request_ip;
    strcpy(request.room, room_name);
    request.length = 0;

    reply = sendUDPrequest(socket, request, serv_addr, serv_addr_size);
    cout << reply.action << ": " << reply.payload << endl;
    if(reply.action == error) exit(1);
    return reply.payload;
}

void launchClient(char *server_ip, int service_port, char *room_name, char *room_pswd, char *ip_pool, int action) {
    char *devName = (char *)pickDevName().c_str();
	cout << devName << endl;

    int tun = tun_alloc(devName);
    /* modifyRoute("172.16.1.0", "255.255.255.0", "172.16.1.1", SIOCADDRT); */

    struct sockaddr_in server = createAddr(server_ip, service_port);
    
    if(action == 0) createRoom(server, room_name, room_pswd, ip_pool);

    configTun(devName, tun, (char *)requestIP(server, room_name, room_pswd).c_str()); 
    sniffer(tun);
}


void launchUDPserver(Server *server, int service_port) {

    
    int socket = openSocket(service_port);

    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);

    while(1) {

        struct datagram packet_in, packet_out;

        memset(&client_addr, 0, client_addr_size);
        memset(&packet_in, 0, sizeof(packet_in));
        memset(&packet_out, 0, sizeof(packet_out));
        
        int recv_size;
        recv_size = recvfrom(socket, &packet_in, sizeof(packet_in), 0, (struct sockaddr*)&client_addr, &client_addr_size);

        if(recv_size == -1) {
            cerr << "Unable to read data" << endl;
            continue;
        }
        if(recv_size != sizeof(packet_in)) {
            cerr << "Data corruption." << endl;
            continue;
        }

        printf("Received message from %s:%d with size %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), recv_size);
        printf("Room: %s\n", packet_in.room);
        printf("Action: %d\n", packet_in.action);
        printf("Length: %d\n", packet_in.length);
        printf("Payload: "); for(int i = 0; i < packet_in.length; i++) printf("%c", packet_in.payload[i]);
        printf("\n");

        if(packet_in.action == create_room) {
            if(server -> rooms[packet_in.room] == NULL) {
                char pswd[64], ip_pool[16];
                sscanf(packet_in.payload, "%[^,],%s", pswd, ip_pool);
                cout << pswd << ": " << ip_pool << endl;
                server -> createRoom(packet_in.room, pswd, ip_pool);

                packet_out.action = success;
                strcpy(packet_out.payload, "Ok!");
                packet_out.length = strlen(packet_out.payload);
            } else {
                packet_out.action = error;
                strcpy(packet_out.payload, "Room exists!");
                packet_out.length = strlen(packet_out.payload);

            }
        } else if(server -> rooms[packet_in.room] == NULL) {
            cout << "Room " << packet_in.room << " not exists." << endl;
            packet_out.action = error;
            strcpy(packet_out.payload, "Room not exists.");
            packet_out.length = strlen(packet_out.payload);
        } else if(packet_in.action == request_ip) {
            unsigned IP = server -> rooms[packet_in.room] -> getIP();
            if(IP != -1) {
                struct in_addr ip_addr;
                ip_addr.s_addr = htonl(IP);

                packet_out.action = offer_ip;
                strcpy(packet_out.payload, inet_ntoa(ip_addr));
                packet_out.length = strlen(packet_out.payload);
            } else {
                packet_out.action = error;
                strcpy(packet_out.payload, "No capability.");
                packet_out.length = strlen(packet_out.payload);
            }
        }
        
        if(sendto(socket, &packet_out, sizeof(packet_out), 0, (struct sockaddr*)&client_addr, client_addr_size) == -1) {
            cerr << "Unable to send message" << endl;;
            exit(1);
        }
    }

}

void launchServer(int service_port) {
    Server *server = new Server();
	launchUDPserver(server, service_port);

}
