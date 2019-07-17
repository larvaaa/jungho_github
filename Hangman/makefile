all: client server

client: client.c
	gcc -o client client.c -pthread

server: server.c
	gcc -o server server.c

clean:
	rm client server

