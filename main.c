/*
 * Interpreter polecen bash-a - projekt zaliczeniowy.
 */

/* 
 * File:   main.c
 * Author: bartosz
 *
 * Created on 18 stycznia 2016, 20:47
 */

#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/*
 * 
 */

void wypisz_polecenie(char **polecenie){
    printf("POLECENIE:");
    while(*polecenie){
        printf(" %s",*polecenie++);
    }   
    printf("\n");    
}

void czysc_polecenie(char ***polecenie){
//    wypisz_polecenie(*polecenie);
    while (*(polecenie+2)) {        
        *(polecenie) = *(polecenie+2);
        *polecenie++; 
    }
    *(polecenie++)=NULL;
    *(polecenie++)=NULL;
//    wypisz_polecenie(*polecenie);
}

int wykonaj_polecenie(char **polecenie){
    char **i;
    char *wyjscie = NULL;
    char *wejscie = NULL;
    int pid;
    int fd_we, fd_wy;
    i = polecenie;
    
    while (*i){
        if(strcmp(*i++,"<")==0){
            wejscie = *i++;
            czysc_polecenie(&*(i-2));
            *i--;
            *i--;
//            wypisz_polecenie(&*polecenie);
        }        
    }
    
    i = polecenie;
    while (*i) {
        if(strcmp(*i++,">")==0){
            wyjscie = *i++;
            czysc_polecenie(&*(i-2));
            *i--;
            *i--;
//            wypisz_polecenie(&*polecenie);
        }
    }
    
//    printf("\n\nOSTETECZNIE:\n");
    wypisz_polecenie(&*polecenie);
    
    if (!(pid=fork())){
        if(wyjscie){
//            printf("WYJŚĆIE: %s \n", wyjscie);
            close(1);
            fd_we = open(wyjscie, O_WRONLY | O_CREAT, 0644);
        }
        if(wejscie){
//            printf("WEJŚĆIE: %s \n", wejscie);
            close(0);
            fd_wy = open(wejscie, O_RDONLY, 0644);
            dup2(0,fd_wy);
        }
        execvp(polecenie[0], polecenie);
    }
    wait(pid);    
    return 0;    
}



char **pobierz_polecenie(char linia[]){
    char **start;    
    char **argumenty;
    argumenty = calloc(sizeof(char*), 128);
    start = argumenty;
    
    int n = strlen(linia);    
    int i = 0;
    
    while(n>0){
        i=0;
        while(isgraph(linia[i])){
            i++;  
        }
        n -= i+1;        
        (*argumenty) = calloc(sizeof(char), i);
        linia[i]='\0';
        strcpy(*argumenty++, linia);
        
//        printf("OSTATNIO POBRANY ARGUMENT: %s\n", *(argumenty-1));
        if(n>0){
            memmove(linia, linia+i+1, sizeof(char) * n); 
            linia[n] = '\0';        
            realloc(linia, sizeof(char) * n);
        }        
    }
    *argumenty = NULL;        
    return start;
}

int main(int argc, char** argv) {

    char *linia;
    char **polecenie;
    int pid;
//    char **tekst;
    while(1){
        linia = readline("~ ~ ~ ~> ");
        polecenie = pobierz_polecenie(linia);    
        wykonaj_polecenie(polecenie);        
    }
    return (EXIT_SUCCESS);
}

