# Makefile para la Práctica 1
all: indexer searcher p1-dataProgram

indexer: src/indexer.c
	gcc src/indexer.c -o bin/indexer

searcher: src/searcher.c
	gcc src/searcher.c -o bin/searcher

p1-dataProgram: src/p1-dataProgram.c
	gcc src/p1-dataProgram.c -o bin/p1-dataProgram

clean:
	rm -f bin/indexer bin/searcher bin/p1-dataProgram bin/req_fifo bin/res_fifo bin/*.bin