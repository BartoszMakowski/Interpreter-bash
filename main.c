/*
 * Interpreter polecen bash-a - projekt zaliczeniowy.
 */

/* 
 * File:   main.c
 * Author: bartosz
 *
 * Created on 18 stycznia 2016, 20:47
 */

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/prctl.h>

#include "moje_jobs.h"
#include "obrobka_tekstu.h"
#include "polecenia.h"

//void domyslne_sygnaly(); // przywraca domyslna obsluge 
//
///*
// * 
// */
//

int main(int args, char** argv) {

    char *linia;
    char *zacheta;
    char **polecenie;
    pid = 0;
    int i;
    int n;
    int *w_tle;
    
    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
//    signal(SIGCLD, SIG_IGN);
    
    for (i=0; i<N; i++){
        jobs[i]=0;
    }
    
    if(args > 1){
        i = open(argv[1], O_RDONLY, 0644);
        dup2(i, 0);
    }
       
    if(isatty(0)){
        zacheta = malloc(sizeof(char)*5);
        strcpy(zacheta,"~~~>");  
    }
    else{
        strcpy(zacheta,"");  
    }
    
    while(linia = readline(zacheta)){
        add_history(linia);
        if (strncmp(linia,"#!",2)){
            polecenie = pobierz_polecenie(linia, &n, argv, w_tle);
            if (n>1){
                pid=fork();
                if (pid == 0){
                    signal(SIGINT, SIG_DFL);
//                    signal(SIGCONT, SIG_IGN);
                    int tpid;
                    tpid = wykonaj_z_potokami(polecenie, n, 0, argv);
                    waitpid(tpid, NULL, 0);
                    exit(0);
                }
                signal(SIGCLD, SIG_DFL);
                signal(SIGINT, przekaz_sygnal);
                waitpid(pid, NULL, 0);
                signal(SIGINT, SIG_IGN);
                signal(SIGCLD, SIG_IGN);
                pid = 0;
            }
            else if (n == 1) {                
                int tmp_pid;
                tmp_pid=wykonaj_polecenie(polecenie, 1, 0, argv);
                if(*w_tle == 0 && tmp_pid != 0){
                    pid = tmp_pid;
                    signal(SIGINT, przekaz_sygnal);
                    signal(SIGTSTP, przekaz_sygnal);
//                    printf("PID: %d\n", pid);
                    waitpid(pid, 0, WUNTRACED);
                    signal(SIGTSTP, SIG_IGN);
                    signal(SIGINT, SIG_IGN);
                    pid = 0;
                }
            }             
        }
        free(linia);
    }
    
    return (EXIT_SUCCESS);
}
