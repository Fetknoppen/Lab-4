all: server client

server: server.o
		$(CXX) -L./ -Wall -o server server.o

client: client.o
		$(CXX) -L./ -Wall -o client client.o

clean:
		rm *.o server client