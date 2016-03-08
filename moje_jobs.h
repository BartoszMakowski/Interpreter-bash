#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>


void dodaj_do_jobs(int, int*, int); // dodaje do listy procesow wstrzymanych
void usun_z_jobs(int, int*, int); // usuwa z listy procesow wstrzymanych
void wypisz_jobs(int*, int); // wypisuje liste procesow wstrzymanych
void moje_fg(int*, int*, int); // aktywuje dotychczas wstrzymany proces w trybie 1-planowym
