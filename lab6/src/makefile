all: server.c client.c
	gcc server.c -L. -lpthread -o server
	gcc client.c -L. -lpthread -o client

clear :
	rm server client