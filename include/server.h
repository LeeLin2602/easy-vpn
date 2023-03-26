#include <bits/stdc++.h>
#include <netio.h>
#include <chrono>
#include <thread>

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
        unsigned getIP() {
            if(pool.empty()) return 0;
            int IP = *pool.begin();
            pool.erase(*pool.begin());
            return network_id + IP;
        }
        void releaseIP(int IP) {
            pool.insert(IP - network_id);
        }
};

class Room {
    private:
        IP_pool *pool;
        string name;
        string pswd;
    public:
        map<pair<string, int>, unsigned> table;
        Room(char *room_name, char *room_pswd, char *pool_ip) {
            pool = new IP_pool(pool_ip);
            name = room_name;
            pswd = room_pswd;
        }
        int authenticate(string password) {
            return password == pswd;
        }
        unsigned getIP(pair<string, int> addr) {
            unsigned res = pool -> getIP();
            if(res == 0) return 0;
            return table[addr] = res;
        }
        int releaseIP(pair<string, int> addr) {
            unsigned IP = table[addr];
            pool -> releaseIP(IP);
            table.erase(addr);
            return table.size();
        }
};

inline int getTimestamp() {
    auto current_time = std::chrono::system_clock::now();
    int timestamp_sec = std::chrono::time_point_cast<std::chrono::seconds>(current_time).time_since_epoch().count();
    return timestamp_sec;
}

class Timer {
    public:
        map<pair<string, int>, int> rec;
        queue<pair<pair<string, int>, int>> que;
        queue<pair<string, int>> timeout;
        void update(pair<string, int> guest) {
            rec[guest] = getTimestamp() + 16;
            que.push({guest, getTimestamp() + 16});
        }
};

class Server {
    public:
        map<string, Room*> rooms;
        map<pair<string, int>, string> guest2room;
        Timer timer;
        void createRoom(char *room_name, char *room_pswd, char *pool_ip) {
            rooms[room_name] = new Room(room_name, room_pswd, pool_ip);
        }
        void join(pair<string, int> guest, string room_name) {
            guest2room[guest] = room_name;
            cout << "Guest " << guest.first << ":" << guest.second << " joins room " << room_name << endl;
            timer.update(guest);
        }
        void recycle() {
            while(1) {
                while(timer.que.size() and timer.que.front().second <= getTimestamp()) {
                    if(getTimestamp() >= timer.rec[timer.que.front().first]) {
                        timer.timeout.push(timer.que.front().first);
                    }
                    timer.que.pop();
                }
                while(timer.timeout.size()) {
                    if(guest2room.count(timer.timeout.front())) {
                        cout << "Guest " << timer.timeout.front().first << ":" << timer.timeout.front().second << " left." << endl;
                        if(rooms[guest2room[timer.timeout.front()]] and !rooms[guest2room[timer.timeout.front()]] -> releaseIP(timer.timeout.front())) {
                            delete rooms[guest2room[timer.timeout.front()]];
                            rooms[guest2room[timer.timeout.front()]] = NULL;
                            cout << "Room " << guest2room[timer.timeout.front()] << " is empty, removed." << endl;
                        }
                        guest2room.erase(timer.timeout.front());
                    }
                    timer.timeout.pop();

                }
                sleep(1);
            }
            
        }
};
