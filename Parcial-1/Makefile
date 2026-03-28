all: backup

backup: src/backup.c
	mkdir -p bin
	mkdir -p data/backup
	gcc src/backup.c -o bin/backup

clean:
	rm -f bin/backup data/backup/archivo-prueba.txt.bak