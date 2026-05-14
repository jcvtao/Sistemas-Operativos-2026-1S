CC = gcc
CFLAGS = -Wall

all: bin p2-server p2-client indexer

bin:
	mkdir -p bin

p2-server: p2-server.c
	$(CC) $(CFLAGS) -o bin/p2-server p2-server.c

p2-client: p2-client.c
	$(CC) $(CFLAGS) -o bin/p2-client p2-client.c

indexer: indexer.c
	$(CC) $(CFLAGS) -o bin/indexer indexer.c

clean:
	rm -rf bin search-history.log