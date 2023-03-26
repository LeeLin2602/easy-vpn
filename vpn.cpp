#include <bits/stdc++.h>
#include <essential.h>

using namespace std;

void print_usage() {
    cout << "As server: ./vpn -s [-port <port>]" << endl;
    cout << "\tDefault port for server is 8964." << endl;
    cout << "As client: ./vpn -c -h <server_ip> -r <room_name> -p <room_password> [-P <ip_pool>] [-port <port>] {create|join}" << endl;
    cout << "\tLength of room_name and room_password must be less than 64 characters." << endl;
    cout << "\tFormat of ip_pool must be like '172.16.0.0' which stands for '172.168.0.0/24' or like '192.168.1.0' which stands for '192.168.1.0/24'. (Only /24 allowed.)" << endl;
    cout << "You have to indicate that you want to create a room or join a room." << endl;
    cout << "GitHub: https://github.com/LeeLin2602/easy-vpn" << endl;
}

void sig_handler(int signum){
    delRoutes();
    if(signum == 2)
        exit(0);
}

int main(int argc, char** argv) {
    signal(SIGINT,sig_handler);
    
    int service = -1;
    int service_port = 8964;
    int action = -1;
    char server_ip[16] = {};
    char room_name[64] = {};
    char room_pswd[64] = {};
    char ip_pool[16] = "172.19.0.0";

    for(int i = 0; i < argc; i++) {
        if(strcmp(argv[i], "-s") == 0) service = 1;
        if(strcmp(argv[i], "-c") == 0) service = 0;
        if(strcmp(argv[i], "-port") == 0 and i + 1 < argc and strlen(argv[i + 1]) < 6) {
            service_port = atoi(argv[i + 1]);
            if(service_port >= 65536 or service_port <= 0) {
                cerr << "Invalid port, using default port = 8964";
                service_port = 8964;
            }
        } 
        if(strcmp(argv[i], "-h") == 0 and i + 1 < argc and strlen(argv[i + 1]) < 16) 
            strncpy(server_ip, argv[i + 1], 15);
        if(strcmp(argv[i], "-r") == 0 and i + 1 < argc and strlen(argv[i + 1]) < 64) 
            strncpy(room_name, argv[i + 1], 63);
        if(strcmp(argv[i], "-p") == 0 and i + 1 < argc and strlen(argv[i + 1]) < 64) 
            strncpy(room_pswd, argv[i + 1], 63);
        if(strcmp(argv[i], "-P") == 0 and i + 1 < argc and strlen(argv[i + 1]) < 16) 
            strncpy(ip_pool, argv[i + 1], 15);
    }
    
    if(!strcmp(argv[argc - 1], "create")) action = 0;
    if(!strcmp(argv[argc - 1], "join")) action = 1;

	/* string x = aesEncrypt("00000000000000000000000asdkjaldaldiasdhlaksdhakljdshaksdjahskdlhljkasdjkjh00000", md5("abc123"), "0123456789abc123"); */
	/* cout << x.size() << ": " << x << endl;; */

    if(service == 1) {
        if(strlen(server_ip) or strlen(room_name) or strlen(room_pswd)) {
            print_usage();
            exit(1);
        }
        launchServer(service_port);
    } else if(service == 0) {
        if(!strlen(server_ip) or !strlen(room_name) or !strlen(room_pswd)) {
            print_usage();
            exit(1);
        }
        launchClient(server_ip, service_port, room_name, room_pswd, ip_pool, action);
    } else {
        cerr << "Unspecified service type." << endl;
        exit(1);
    }

    return 0;
}
