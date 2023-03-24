#include <bits/stdc++.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <net/if.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <linux/route.h>
#include "boost/asio/io_service.hpp"
#include "boost/asio/ip/address.hpp"
#include "boost/asio/ip/udp.hpp"

using namespace std;



/* boost::asio::ip::address source_address( */
/*   const boost::asio::ip::address& ip_address) { */
/*   using boost::asio::ip::udp; */
/*   boost::asio::io_service service; */
/*   udp::socket socket(service); */
/*   udp::endpoint endpoint(ip_address, 3478); */
/*   socket.connect(endpoint); */
/*   return socket.local_endpoint().address(); */
/* } */


string pickDevName();
int tun_alloc(char *dev);
void configTun(char* devName, int tun);
void modifyRoute(string dst, string mask, string gw, int opr);
void delRoutes();
