# Makefile para la Práctica 1
CC=gcc
CFLAGS=-Iinclude

all: indexer searcher

indexer: src/indexer.c
	$(CC) src/indexer.c -o bin/indexer

searcher: src/searcher.c
	$(CC) src/searcher.c -o bin/searcher

clean:
	rm -f bin/indexer bin/searcher bin/*.bin