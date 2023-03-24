vpn : vpn.cpp
	g++ -o $@ $^ -g -Wall -l pthread

test:
	make
	./vpn

clean :
	rm vpn
