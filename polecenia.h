#define N 100

#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>


#include "moje_jobs.h"
#include "obrobka_tekstu.h"

int pid;
int fd[100][2];
int jobs[N];

void wykonaj(char**); // wykonanie komendy lub echo
int wykonaj_polecenie(char**, int, int, char**); // 
void przekaz_sygnal(int); // przekazuje sygnal do odpowiedniego procesu
int wykonaj_z_potokami(char**, int, int, char**); // wykonuje ciag polecen polaczonych potokami
void domyslne_sygnaly(); // przywarace domyslna obsluge SIGCLD, SIGINT, SIGTSTP

