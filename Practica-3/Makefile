all: bin p2-server p2-client indexer

bin:
	mkdir -p bin

indexer: indexer.c
	gcc -Wall -Wextra -pedantic -o bin/indexer indexer.c

p2-server: p2-server.c
	gcc -Wall -Wextra -pedantic -o bin/p2-server p2-server.c

p2-client: p2-client.c
	gcc -Wall -Wextra -pedantic -o bin/p2-client p2-client.c

clean:
	rm -f bin/indexer bin/p2-server bin/p2-client bin/*.bin bin/server.log