
LIBDIR = lib/
LIB := $(shell find $(LIBDIR) -name '*.cpp')

vpn : vpn.cpp $(LIB)
	g++ -o $@ $^ -g -Wall -l pthread -I include

test:
	make
	./vpn

clean :
	rm vpn
