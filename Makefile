#CC=g++ -g -O3 -DNDEBUG
CC=g++ -g -std=c++17 -O3
PROTOC=protoc

PROTOBUF=./protobuf-3.18.1/src
LIB=$(PROTOBUF)/.libs/libprotobuf.a -ldl -pthread
INC=-I $(PROTOBUF)

COMMON_O=kv.pb.o log.o protocol.o rpc.o

all: client server

# binaries and main object files

test: client server
	export VERBOSITY=1
	./server 4242

debug: client server
	export VERBOSITY=1
	gdb --eval-command=r --args server 4242

client: client.o common
	$(CC) -o client client.o $(COMMON_O) $(LIB)

client.o: client.cpp common
	$(CC) -c client.cpp $(INC)

server: server.o common storage.o table.bin
	$(CC) -o server server.o storage.o $(COMMON_O) $(LIB)

table.bin:
	touch table.bin

server.o: server.cpp
	$(CC) -c server.cpp $(INC)
	
storage.o: storage.cpp
	$(CC) -c storage.cpp $(INC)

# libs

common: kv.pb.o log.o protocol.o rpc.o

log.o: log.h log.cpp
	$(CC) -c log.cpp $(INC)

kv.pb.o: kv.proto
	$(PROTOC) --cpp_out=. kv.proto
	$(CC) -c kv.pb.cc $(INC)

protocol.o: protocol.h protocol.cpp
	$(CC) -c protocol.cpp $(INC)

rpc.o: rpc.h rpc.cpp
	$(CC) -c rpc.cpp $(INC)
