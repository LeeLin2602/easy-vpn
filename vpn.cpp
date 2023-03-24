#include <bits/stdc++.h>
#include <netconf.h>
#include <netio.h>

using namespace std;



void sig_handler(int signum){
    delRoutes();
    if(signum == 2)
        exit(0);
}

void sniffer(int tun){
    const int buf_size = 1024 * 64;
    char buf[buf_size];

	while(1) {
        int n = read(tun, buf, buf_size);
        cout << n << ": " << buf << endl;
	}
}

int main() {
    signal(SIGINT,sig_handler);
    
    char *devName = (char *)pickDevName().c_str();
    int tun = tun_alloc(devName);
	configTun(devName, tun);
    modifyRoute("2.2.2.0", "255.255.255.0", "172.16.1.1", SIOCADDRT);

    // netio_test();
    // sniffer(tun);
    
    return 0;
}
