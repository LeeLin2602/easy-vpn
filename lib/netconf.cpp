#include <netconf.h>

using namespace std;

vector<pair<pair<string, string>, string>> addedRoutes;

string pickDevName() {
    set<string> devNames;

    struct ifaddrs * addresses;

    if (getifaddrs( & addresses) == -1) {
        cerr << "getifaddrs call failed\n";
        exit(1);
    }

    struct ifaddrs * address = addresses;
    while (address) {
        int family = address -> ifa_addr -> sa_family;
        if (family == AF_INET || family == AF_INET6)
            devNames.insert(address -> ifa_name);
        address = address -> ifa_next;
    }
    freeifaddrs(addresses);
    for(int i = 0;; i++) {
        string devName = "tun"; devName += to_string(i);
        if(devNames.count(devName) == 0)
            return devName;
    }
}

int tun_alloc(char *dev) {

#define CHECKAUX(e,s)                            \
	((e)? \
	(void)0: \
	(fprintf(stderr, "'%s' failed at %s:%d - %s\n", \
	s, __FILE__, __LINE__,strerror(errno)), \
	exit(0)))
#define CHECK(e) (CHECKAUX(e,#e))
#define CHECKSYS(e) (CHECKAUX((e)==0,#e))
#define CHECKFD(e) (CHECKAUX((e)>=0,#e))
#define STRING(e) #e

	int fd = open("/dev/net/tun", O_RDWR);
	CHECKFD(fd);

	struct ifreq ifr; 
	memset(&ifr, 0, sizeof(ifr)); 
	ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
	strncpy(ifr.ifr_name, dev, IFNAMSIZ); 
	CHECKSYS(ioctl(fd, TUNSETIFF, (void *) &ifr));
	strncpy(dev, ifr.ifr_name, IFNAMSIZ); 
	return fd;
}

    /* auto destination_address = boost::asio::ip::address::from_string("8.8.8.8"); */
    /* std::cout << "Source ip address: " */
    /*     << source_address(destination_address).to_string() */
    /*     << '\n'; */

void configTun(char* devName, int tun) {
    int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
    struct ifreq ifr;
    strncpy(ifr.ifr_name, devName, IFNAMSIZ);

    struct sockaddr_in* addr = (struct sockaddr_in*)&ifr.ifr_addr;
    ifr.ifr_addr.sa_family = AF_INET;

    inet_pton(AF_INET, "172.16.1.1", &addr->sin_addr);
    ioctl(fd, SIOCSIFADDR, &ifr);

    inet_pton(AF_INET, "255.255.255.0", &addr->sin_addr);
    ioctl(fd, SIOCSIFNETMASK, &ifr);

    ifr.ifr_flags |= (IFF_UP | IFF_RUNNING);
    ioctl(fd, SIOCSIFFLAGS, &ifr);

    close(fd);
}

void modifyRoute(string dst, string mask, string gw, int opr) {
    int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

    struct rtentry route;
    memset(&route, 0, sizeof(route));
    
    struct sockaddr_in * addr;

    addr = (struct sockaddr_in*) &route.rt_gateway;
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr((char *)gw.c_str());

    addr = (struct sockaddr_in*) &route.rt_dst;
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr((char *)dst.c_str());

    addr = (struct sockaddr_in*) &route.rt_genmask;
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr((char *)mask.c_str());

    route.rt_flags = RTF_UP | RTF_GATEWAY;
    route.rt_metric = 0;

    if(ioctl(fd, opr, &route) < 0) {
        cerr << strerror(errno) << endl;
        exit(1);
    }

    if(opr == SIOCADDRT) {
        addedRoutes.push_back({{dst, mask}, gw});
        cout << "add route " << dst << " " << mask << " via " << gw << endl;
    }

    close(fd);
}

void delRoutes(){
    for(auto [src, gw]: addedRoutes) {
        auto [ip, mask] = src;
        cout << "remove route " << ip << " " << mask << " via " << gw << endl;
        modifyRoute(ip, mask, "10.1.1.254", SIOCDELRT);
    }
}
