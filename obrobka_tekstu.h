#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void zamien_argumenty(char**, char**); // podstawia wartosci zmiennych podanych jako argumenty
void wypisz_polecenie(char**); // wypisuje polecenie
void czysc_polecenie(char***); // usuwa przekierowania znajdujace sie wsrod argumentow
char **pobierz_polecenie(char*, int*, char**, int*); // pobiera pojedyncze polecenie z wczytanej linii
void ustaw_zmienna(char*); // ustawia podane zmienne srodowiskowe
