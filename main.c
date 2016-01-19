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

/*
 * 
 */



void pobierzPolecenie(char linia[]){
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
    execvp(start[0],start);
}

int main(int argc, char** argv) {
//    char *tab[] = {"ls","-la", NULL};
//    execvp(tab[0],tab);
    char *line;
    int pid;
//    char **tekst;
    while(1){
        line = readline("~ ~ ~ ~>");
        if (!(pid=fork())){
            pobierzPolecenie(line);
        }
        wait(pid);
        
    }
    return (EXIT_SUCCESS);
}

