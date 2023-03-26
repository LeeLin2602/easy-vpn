# easy-vpn

My goal is to develop a simple vpn that allows multi-connections.

Requirement:
```
# sudo apt-get install libboost-all-dev libssl-dev
```

Usage:
```
As server: ./vpn -s [-port <port>]
        Default port for server is 8964.
As client: ./vpn -c -h <server_ip> -r <room_name> -p <room_password> [-P <ip_pool>] [-port <port>] {create|join}
        Length of room_name and room_password must be less than 64 characters.
        Format of ip_pool must be like '172.16.0.0' which stands for '172.168.0.0/24' or like '192.168.1.0' which stands for '192.168.1.0/24'. (Only /24 allowed.)
You have to indicate that you want to create a room or join a room.
```
