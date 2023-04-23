# easy-vpn

My goal is to develop a simple vpn that allows multi-connections.

Requirement:
```
# sudo apt-get install libboost-all-dev libssl-dev
# make
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

## 原理

### VPN 原理

在 `lib/netconf.cpp` 中，有 `tun_alloc` 函式可以新增一條 tunnel interface；同檔案中，有 `configTun` 可以設定 tunnel interface 的基本參數。

在 `lib/essential.cpp` 中，`sniffer` 函式會去攔截所有從此 tunnel interface 出去的封包，並且發給 server。
在 `lib/essential.cpp` 中，`UDPClient` 函式會去從 server 接收封包，解碼後寫回 tunnel。

### 密碼學原理

#### 新增房間

新增房間的時候會將房間密碼的 2 次 SHA256 結果明文傳輸給伺服器。
（note：此處可能被竊聽）

#### 加入房間

加入房間時候也要將房間密碼的 2 次 SHA256 結果明文傳輸給伺服器做認證。
由於密碼可能會被竊聽，因此加入房間並不意味此人是安全的。

#### 資料傳輸

資料是以房間密碼的 MD5 值作爲金鑰進行 AES 加密傳輸。
因此沒有密碼的人哪怕加入房間，也無法順利加解密封包。
（note：此處存在回放攻擊）

#### 安全性評估

安全性比不安全的 LAN 來的高（如公有 Wi-Fi），但又遜於完全隔離的 L2 網路。
