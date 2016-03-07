#include <sys/types.h>
#include <sys/wait.h>


void dodaj_do_jobs(int, int*, int);
void usun_z_jobs(int, int*, int);
void wypisz_jobs(int*, int);
int moje_fg(int*, int*, int);
