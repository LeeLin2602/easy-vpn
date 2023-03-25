#include <essential.h>

void sniffer(int tun){
    const int buf_size = 1024 * 64;
    char buf[buf_size];

    while(1) {
        int n = read(tun, buf, buf_size);
        cout << n << ": " << buf << endl;
    }
}

void launchClient() {
    char *devName = (char *)pickDevName().c_str();
    int tun = tun_alloc(devName);
    configTun(devName, tun);
    /* modifyRoute("172.16.1.0", "255.255.255.0", "172.16.1.1", SIOCADDRT); */

    sendIPrequest(); 
}

void sendIPrequest() {
    int socket = openSocket(-1);

    struct sockaddr_in server_addr = createAddr((char *)"0.0.0.0", 8964);
    socklen_t server_addr_size = sizeof(server_addr);
    struct datagram request, reply;
    
    request.action = request_ip;
    strcpy(request.room, "abc123");
    request.length = 0;

    reply = sendUDPrequest(socket, request, server_addr, server_addr_size);
    cout << reply.action << ": " << reply.payload << endl;
}

void launchUDPserver() {

    int socket = openSocket(8964);

    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);

    while(1) {

        struct datagram packet_in, packet_out;

        memset(&client_addr, 0, client_addr_size);
        memset(&packet_in, 0, sizeof(packet_in));
        
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

        packet_out.action = offer_ip;
        strcpy(packet_out.payload, "172.168.1.1");
        
        if(sendto(socket, &packet_out, sizeof(packet_out), 0, (struct sockaddr*)&client_addr, client_addr_size) == -1) {
            cerr << "Unable to send message" << endl;;
            exit(1);
        }
    }

}

void launchServer() {
	launchUDPserver();

}
