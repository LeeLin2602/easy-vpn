#include <netio.h>

using namespace std;


int openSocket(int port) {
    int serv_sock;
    struct sockaddr_in serv_addr;
    serv_sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (serv_sock == -1) {
        cerr << "Unable to open socket." << endl;
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        cerr << "Unable to bind 0.0.0.0:" << port << endl;
        exit(1);
    }

	return serv_sock;
}

/* inline int send(int socket, string message, struct sockaddr_in dest_addr, socklen_t dest_addr_size) { */
/* 	return sendto(socket, message.c_str(), message.size(), 0, (struct sockaddr*)&dest_addr, &dest_addr_size); */
/* } */

void netio_test() {

    const int buf_size = 1024 * 64;
    char buf[buf_size];

    int socket = openSocket(8888);
    
    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);

    while(1) {
        
        memset(buf, 0, buf_size);
        memset(&client_addr, 0, client_addr_size);

        int recv_size;
        recv_size = recvfrom(socket, buf, buf_size, 0, (struct sockaddr*)&client_addr, &client_addr_size);
        
        if(recv_size == -1) {
            perror("Unable to read data");
            continue;
        }

        printf("Received message from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        printf("Message: %s\n", buf);

        if(sendto(socket, buf, recv_size, 0, (struct sockaddr*)&client_addr, client_addr_size) == -1) {
            cerr << "Unable to send message" << endl;;
            exit(1);
        }
    }
}
