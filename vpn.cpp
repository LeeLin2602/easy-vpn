#include <bits/stdc++.h>
#include <netconf.h>
#include <netio.h>
#include <essential.h>

using namespace std;

void sig_handler(int signum){
    delRoutes();
    if(signum == 2)
        exit(0);
}

int main(int argc, char** argv) {
    signal(SIGINT,sig_handler);
    
    int service = -1;
    for(int i = 0; i < argc; i++) {
        if(strcmp(argv[i], "-s") == 0) service = 1;
        if(strcmp(argv[i], "-c") == 0) service = 0;
    }
    
    if(service == 1)
        launchServer();
    else if(service == 0)
        launchClient();
    else {
        cerr << "Unspecified service type." << endl;
        exit(1);
    }

    return 0;
}
