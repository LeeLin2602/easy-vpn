
LIBDIR = lib/
LIB := $(shell find $(LIBDIR) -name '*.cpp')
args = `arg="$(filter-out $@,$(MAKECMDGOALS))" && echo $${arg:-${1}}`


vpn : vpn.cpp $(LIB)
	g++ -o $@ $^ -g -Wall -l pthread -I include --std=c++17

test_server:
	make 
	./vpn -s
	
test_client:
	make 
	./vpn -c -h 0.0.0.0 -r test_room -p abc123 create

clean :
	rm vpn
