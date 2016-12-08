# Marc Stocker
# Kevin Okeke
CC = gcc
CFLAGS = -Wall

all: client.exe server.exe

client.exe: client
	$(CC) $(CFLAGS) -o client client.c

server.exe: server
	$(CC) $(CFLAGS) -o server server.c

clean:
	rm -f server client
