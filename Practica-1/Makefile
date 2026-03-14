# Makefile para la Práctica 1
all: indexer searcher menu

indexer: src/indexer.c
	gcc src/indexer.c -o bin/indexer

searcher: src/searcher.c
	gcc src/searcher.c -o bin/searcher

menu: src/menu.c
	gcc src/menu.c -o bin/menu

clean:
	rm -f bin/indexer bin/searcher bin/menu bin/*.bin