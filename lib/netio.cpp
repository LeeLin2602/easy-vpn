#include <netio.h>

using namespace std;

struct sockaddr_in createAddr(char *ip, int port){
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_aton(ip, &addr.sin_addr) == 0) {
        cerr << "inet_aton() failed" << endl;
        exit(1);
    }
    return addr;
}

int openSocket(int port) {
    int serv_sock;
    struct sockaddr_in serv_addr;
    serv_sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (serv_sock == -1) {
        cerr << "Unable to open socket." << endl;
        exit(1);
    }

    if(port != -1) {
        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr = createAddr((char *)"0.0.0.0", port);
        
        if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
            cerr << "Unable to bind 0.0.0.0:" << port << endl;
            exit(1);
        }
    }
	return serv_sock;
}

inline void sendUDP(int socket, struct datagram request, struct sockaddr_in server_addr, socklen_t &server_addr_size) {
    sendto(socket, &request, sizeof(request), 0, (struct sockaddr*)&server_addr, server_addr_size);
}

struct datagram sendUDPrequest(int socket, struct datagram request, struct sockaddr_in server_addr, socklen_t &server_addr_size) {
    struct datagram reply;
    fd_set read_fds;
    struct timeval timeout;
    
    FD_ZERO(&read_fds);
    FD_SET(socket, &read_fds);
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    
    while(1) {
        sendUDP(socket, request, server_addr, server_addr_size);
        if(select(socket + 1, &read_fds, NULL, NULL, &timeout) <= 0) 
            cerr << "Unable to communicate with server. Retrying." << endl;
        else {
            struct sockaddr_in serv_addr;
            socklen_t serv_addr_size;
            int recv_size = recvfrom(socket, &reply, sizeof(reply), 0, (struct sockaddr*)&serv_addr, &serv_addr_size);
            if(recv_size < -1) {
                cerr << "recvform failed. Retrying" << endl;
                continue;
            }
            if(recv_size != sizeof(reply)) {
                cerr << "Data corruption. Retrying" << endl;
                continue;
            }
            return reply;
        }
    }
}
