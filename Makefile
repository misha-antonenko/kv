#CC=g++ -g -O3 -DNDEBUG
CC:=clang++ -std=c++17 -O3
PROTOC:=./protoc

PROTOBUF:=./protobuf-3.18.1/src
LIB:=$(PROTOBUF)/.libs/libprotobuf.a -ldl -pthread -lstdc++fs 
INC:=-I $(PROTOBUF) -I include -Isrc
COMMON_O:=bin/kv.pb.o bin/log.o bin/protocol.o bin/rpc.o

all: client server

test: client server
	VERBOSITY=1 ./server 4242

debug: client server
	export VERBOSITY=1
	gdb --eval-command=r --args server 4242

client: bin/client.o common
	$(CC) -o client bin/client.o $(COMMON_O) $(LIB)

server: bin/server.o common bin/storage.o
	$(CC) -o server bin/server.o bin/storage.o $(COMMON_O) $(LIB)

common: $(COMMON_O)

bin/%.o: src/%.cpp
	$(CC) -o $@ -c $< $(INC)
	
bin/%.pb.o: src/%.pb.cc
	$(CC) -o $@ -c $< $(INC)

clean_base:
	rm bin/my_first_base.*

clean:
	rm bin/*