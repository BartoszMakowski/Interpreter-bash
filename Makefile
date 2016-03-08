CC=gcc
CFLAGS=-Wall

all: my_shell.out

my_shell.out: obrobka_tekstu.o moje_jobs.o polecenia.o main.o
	$(CC) $(CFLAGS) obrobka_tekstu.o moje_jobs.c polecenia.o main.o -o my_shell.out -lreadline

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o -lreadline

obrobka_tekstu.o: obrobka_tekstu.c
	$(CC) $(CFLAGS) -c obrobka_tekstu.c -o obrobka_tekstu.o

moje_jobs.o: moje_jobs.c
	$(CC) $(CFLAGS) -c moje_jobs.c -o moje_jobs.o
	
polecenia.o: polecenia.c
	$(CC) $(CFLAGS) -c polecenia.c -o polecenia.o
	
clean:
	rm *.o my_shell.out
