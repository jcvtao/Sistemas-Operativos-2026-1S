# Makefile para la Práctica 1
all: bin indexer searcher menu p1-dataProgram

bin:
	mkdir -p bin

indexer: src/indexer.c
	gcc src/indexer.c -o bin/indexer

searcher: src/searcher.c
	gcc src/searcher.c -o bin/searcher

menu:
	gcc src/menu.c -o bin/menu	

p1-dataProgram: src/p1-dataProgram.c
	gcc src/p1-dataProgram.c -o p1-dataProgram

clean:
	rm -f bin/indexer bin/searcher bin/menu p1-dataProgram bin/req_fifo bin/res_fifo bin/*.bin