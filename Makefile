
LIBDIR = lib/
LIB := $(shell find $(LIBDIR) -name '*.cpp')

# CRYPTOLIB = cryptopp870/
# CRYPTO := $(shell find $(CRYPTODIR) -name '*.cpp')



vpn : vpn.cpp $(LIB) 
	g++ -o $@ $^ -g -Wall -l pthread -I include -lssl -lcrypto --std=c++17

test_server:
	make 
	./vpn -s
	
test_client:
	make 
	./vpn -c -h 0.0.0.0 -r test_room -p abc123 create

test_client_join:
	make 
	./vpn -c -h 127.0.0.1 -r test_room -p abc123 join

clean :
	rm vpn
