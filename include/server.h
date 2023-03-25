#include<bits/stdc++.h>

using namespace std;

class IP_pool {
    private:
        set<int> pool;
        int network_id = 0;
    public:
        IP_pool(char *pool_ip){
            int ip[4];
            sscanf(pool_ip, "%d.%d.%d.%d", ip, ip + 1, ip + 2, ip + 3);
            for(int i = 0; i < 4; i++) {
                if(ip[i] >= 255 or ip[i] < 0) {
                    ip[0] = 172;
                    ip[1] = 19;
                    ip[2] = 1;
                    ip[3] = 0;
                    break;
                }
            }
            for(int i = 0; i < 4; i++) network_id = network_id * 256 + ip[i];
            for(int i = 1; i <= 254; i++) pool.insert(i);
        }
        int getIP() {
            if(pool.empty()) return -1;
            int IP = *pool.begin();
            pool.erase(*pool.begin());
            return network_id + IP;
        }
        /* void releaseIP(int IP) { */
            
        /* } */
};

class Room {
    private:
        IP_pool *pool;
        string name;
        string pswd;
    public:
        Room(char *room_name, char *room_pswd, char *pool_ip) {
            pool = new IP_pool(pool_ip);
            name = room_name;
            pswd = room_pswd;
        }
        int getIP() {
            return pool -> getIP();
        }
};

class Server {
    public:
        map<string, Room*> rooms;

        void createRoom(char *room_name, char *room_pswd, char *pool_ip) {
            rooms[room_name] = new Room(room_name, room_pswd, pool_ip);
        }
};
