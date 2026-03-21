all: mutex-1 mutex-2

mutex-1: taller-mutex-1.c
	gcc taller-mutex-1.c -o taller-mutex-1 -lpthread

mutex-2: taller-mutex-2.c
	gcc taller-mutex-2.c -o taller-mutex-2 -lpthread

clean:
	rm -f taller-mutex-1 taller-mutex-2