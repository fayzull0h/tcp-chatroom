all: server client

server: server.c commonutils.c commonutils.h
	gcc server.c commonutils.c -o server -lncurses

client: client.c commonutils.c commonutils.h
	gcc client.c commonutils.c -o client -lncurses