#include <essential.h>

string hashDatagram(struct datagram &data) {
	string message;
	for(int i = 0; data.room[i]; i++) message += data.room[i];
    for(int i = 0; i < data.length; i++) message += data.payload[i];
    message += to_string(data.action);
    /* message += ","; */
    /* message += to_string(data.ip); */
    return sha256(message);
}

void sniffer(int socket, int tun, struct sockaddr_in server_addr, char *room_name, char *room_pswd){

    const int buf_size = 1024 * 64;
    char buf[buf_size];

    while(1) {

        int n = read(tun, buf, buf_size);
        string frame; for(int i = 0; i < n; i++) frame.push_back(buf[i]);
		frame = aesEncrypt(frame, md5(room_pswd), "0000000000000000");
		struct datagram traffic; memset(&traffic, 0, sizeof(traffic));
        strcpy(traffic.room, room_name);
	    traffic.action = forward_traffic;	
        for(long unsigned int i = 0; i < frame.size(); i ++) traffic.payload[i] = frame[i];
        traffic.length = frame.length();

        struct sockaddr_in serv_addr = server_addr;
        socklen_t serv_addr_size = sizeof(serv_addr);

        sendUDP(socket, traffic, serv_addr, serv_addr_size);
    }
}

void createRoom(int socket, struct sockaddr_in server_addr, char *room_name, char *room_pswd, char *ip_pool) {

    struct sockaddr_in serv_addr = server_addr;
    socklen_t serv_addr_size = sizeof(serv_addr);
    struct datagram request, reply;
    
    string message = room_pswd;
    message = sha256(sha256(room_pswd));
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

string requestIP(int socket, struct sockaddr_in server_addr, char *room_name, char *room_pswd) {

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

void UDPClient(int socket, int tun, char *server_ip, char *pswd) {
    
    struct sockaddr_in server_addr;
    socklen_t server_addr_size = sizeof(server_addr);

    while(1) {
        struct datagram packet_in;

        memset(&server_addr, 0, server_addr_size);
        memset(&packet_in, 0, sizeof(packet_in));
        
        int recv_size;
        recv_size = recvfrom(socket, &packet_in, sizeof(packet_in), 0, (struct sockaddr*)&server_addr, &server_addr_size);

        if(recv_size == -1) {
            cerr << "Unable to read data" << endl;
            continue;
        }
        if(recv_size != sizeof(packet_in)) {
            cerr << "Data corruption." << endl;
            continue;
        }

        /* if(strcmp(inet_ntoa(server_addr.sin_addr), server_ip)) { */
        /*     cerr << "Invalid sender." << endl; */
        /*     cerr << "Received from " << inet_ntoa(server_addr.sin_addr) << ", " << server_ip << " was expected." << endl; */ 
        /*     continue; */
        /* } */

        if(strcmp(packet_in.checksum, hashDatagram(packet_in).c_str())) {
            // corruption, drop the packet
            cerr << "Corruption." << endl;
            printf("Checksum1: %s\n", packet_in.checksum);
            printf("Checksum2: %s\n", hashDatagram(packet_in).c_str());
            continue;
        }

        string frame; for(int i = 0; i < packet_in.length; i++) frame.push_back(packet_in.payload[i]);
		frame = aesDecrypt(frame, md5(pswd), "0000000000000000");
        write(tun, (char *)frame.c_str(), frame.size());

    }
}

void launchClient(char *server_ip, int service_port, char *room_name, char *room_pswd, char *ip_pool, int action) {
    int socket = openSocket(-1);

    char *devName = (char *)pickDevName().c_str();
	cout << devName << endl;

    int tun = tun_alloc(devName);
    /* modifyRoute("172.16.1.0", "255.255.255.0", "172.16.1.1", SIOCADDRT); */

    struct sockaddr_in server = createAddr(server_ip, service_port);
    
    if(action == 0) createRoom(socket, server, room_name, room_pswd, ip_pool);

    configTun(devName, tun, (char *)requestIP(socket, server, room_name, room_pswd).c_str()); 
    
    thread UDPClientThread(UDPClient, socket, tun, server_ip, room_pswd);
    sniffer(socket, tun, server, room_name, room_pswd);

    UDPClientThread.join();
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
        printf("Payload: "); for(int i = 0; i < packet_in.length; i++) printf("%c", packet_in.payload[i]); printf("\n");
        if(strcmp(packet_in.checksum, hashDatagram(packet_in).c_str())) {
            // corruption, drop the packet
            cerr << "Corruption." << endl;
            printf("Checksum1: %s\n", packet_in.checksum);
            printf("Checksum2: %s\n", hashDatagram(packet_in).c_str());
            continue;
        }
        if(packet_in.action == create_room) {
            if(server -> rooms[packet_in.room] == NULL) {
                char pswd[65], ip_pool[16];
                for(int i = 0; i < 64; i++) 
                    pswd[i] = packet_in.payload[i];
                for(int i = 65; packet_in.payload[i]; i++) {
                    ip_pool[i - 65] = packet_in.payload[i];
                }
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
            unsigned IP = server -> rooms[packet_in.room] -> getIP(client_addr);
            if(IP != 0) {
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
        } else if(packet_in.action == forward_traffic) {
            for(auto [ip, addr]: server -> rooms[packet_in.room] -> table) {
                struct sockaddr_in guest = createAddr(inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
                socklen_t guest_size = sizeof(guest);
                sendUDP(socket, packet_in, guest, guest_size);
            }
        }
        
        sendUDP(socket, packet_out, client_addr, client_addr_size);
    }

}

void launchServer(int service_port) {
    Server *server = new Server();
	launchUDPserver(server, service_port);

}
