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
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

/*
 * 
 */

int pid;
int w_tle;
int fd[100][2];

void przekaz_sygnal(int signo){
    printf("Przesyłanie sygnału >>%i<< do procesu: >>PID: %i<<\n", signo, pid);
    kill(pid, signo);
}

void obsluga_procesu_potomnego(int signo){
    int s, pid2;
    if (pid2 = waitpid(pid,&s,0) <=0){ 
        pid2 = waitpid(-1,&s,0);
        printf("PID %d: status zakończenia: %d\n", pid2, s>>8);
    }
    else{
        printf("PID %d: status zakończenia: %d\n", pid, s>>8);
    }    
}

void wypisz_polecenie(char **polecenie){
//    printf("POLECENIE:");
//    while(*polecenie){
//        printf(" %s",*polecenie++);
//    }   
//    printf("\n");    
}

void czysc_polecenie(char ***polecenie){
    while (*(polecenie+2)) {        
        *(polecenie) = *(polecenie+2);
        *polecenie++; 
    }
    *(polecenie++)=NULL;
    *(polecenie++)=NULL;
}

int wykonaj_polecenie(char **polecenie, int n, int k){
    signal(SIGINT,przekaz_sygnal);        
    int tlo = 0;
    char **i = calloc(sizeof(char*), 128);
    char **start;
    start = i;
    char dopisz = 0;
    char *wyjscie = NULL;
    char *wejscie = NULL;
//    int pid;
    int fd_we, fd_wy;    
    char *tmp;
    
    tmp = strtok(*polecenie, " ");
    while(tmp){
//        printf("TMP: %s", tmp);
        (*i) = calloc(sizeof(char), strlen(tmp));
        strcpy(*i++, tmp);
        tmp = strtok(NULL, " ");
    }
    
    i = start;
    
    if(strcmp(*i,"exit") == 0){
        exit(0);
    }
    
    while (*i) {
        if(strcmp(*i,">")==0){
            wyjscie = *++i;
            czysc_polecenie(&*(i-1));
            *i--;
//            wypisz_polecenie(&*polecenie);
        } else if(strcmp(*i,">>")==0){
            dopisz = 1;
            wyjscie = *++i;
            czysc_polecenie(&*(i-1));            
            *i--;
//            wypisz_polecenie(&*polecenie);
        } else if(strcmp(*i++,"<")==0){
            wejscie = *i++;
            czysc_polecenie(&*(i-2));
            *i--;
            *i--;
//            wypisz_polecenie(&*polecenie);
        }        
    }
    

    
    if(strcmp(*--i,"&")==0){
        *i = NULL;
        free(*i);
        tlo = 1;
    }
    
    i = start;
    
    wypisz_polecenie(&*i); 
        
    if ((pid=fork())==0){
            if(k > 0){
                dup2(fd[k-1][0], 0);
                close(fd[k-1][0]);
            }
            if(n>1){
                dup2(fd[k][1], 1);
                close(fd[k][1]);
            }
        if(wyjscie){
//            printf("WYJŚĆIE: %s \n", wyjscie);
            close(1);
            switch (dopisz){
                case 0:
                    fd_wy = open(wyjscie, O_WRONLY | O_CREAT, 0644);
                    break;
                case 1:
                    fd_wy = open(wyjscie, O_WRONLY | O_CREAT | O_APPEND, 0644);
                    break;                    
            }            
        }
        if(wejscie){
//            printf("WEJŚĆIE: %s \n", wejscie);
            close(0);
            fd_wy = open(wejscie, O_RDONLY, 0644);
            dup2(0,fd_we);
        }
        
        if(!tlo){
            execvp(i[0], i);
        }
        else{
            int pid_tlo;
            int status;
            pid_tlo = fork();
            if(pid_tlo == 0){
                execvp(i[0], i);                
            }
            else{
                waitpid(pid_tlo, &status, 0);
                printf("PID: %d | STATUS ZAKOŃCZENIA: %d\n", pid_tlo, status);                
            }
        }
//        
//        close(fd[1]);
        exit(0);
    }
    else{
        close(fd[k][1]);
        if(!tlo){
            int status;
            waitpid(pid, &status, 0);
//            printf("PID: %d | STATUS ZAKOŃCZENIA: %d\n", pid, status);             
        }
    }

    return 0;    
}



char **pobierz_polecenie(char *linia, int *n){
    char **start;    
    char **argumenty;
    argumenty = calloc(sizeof(char*), 128);
    start = argumenty;
    char *tmp;
    int i=0;
    
    tmp = strtok(linia, "|");
    while(tmp){
//        printf("%s  ", tmp);
        (*argumenty) = calloc(sizeof(char), strlen(tmp));
        strcpy(*argumenty++, tmp);
        tmp = strtok(NULL, "|");
//        (*n)++;
        i++;
    }
    *n = i;
    return start;
}

int policz_komendy(char *linia){
    int i=0;
    char *tmp;
    tmp = strtok(linia, "|");
    while(tmp){
        tmp = strtok(NULL, "|");
        i++;
    }
    return i;    
}

int main(int args, char** argv) {

    char *linia;
    char **polecenie;
    int pid;
    int i;
    int n;
    w_tle = 0;
       

    while(linia = readline("~ ~ ~ ~> ")){
        add_history(linia);
        polecenie = pobierz_polecenie(linia, &n);
        for(i=0; i<n; i++){
            pipe(fd[i]);
        }
        i = 0;
//            printf("KOMENDY: %d\n", n);
        while(*polecenie != NULL){
//            printf("%s\n", *polecenie++);
            wykonaj_polecenie(polecenie++, n--, i++);            
        }
    }
    
    return (EXIT_SUCCESS);
}

