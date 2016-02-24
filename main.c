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
int pid_tab[100];
int jobs[100];

void przekaz_sygnal(int signo){
//    printf("Przesyłanie sygnału >>%i<< do procesu: >>PID: %i<<\n", signo, pid);
    printf("%d\n", signo);
    if(signo == SIGTSTP){
        kill(pid, SIGSTOP);
    }
    else{
        kill(pid, signo);
    }
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

void wykonaj(char **polecenie){
    if(strcmp(polecenie[0], "echo") == 0){
        char **i;
        i = polecenie;
        while(*++i){
            printf("%s ",*i);
        }
    }
    else{
        execvp(polecenie[0], polecenie);
    }
}

int wykonaj_polecenie(char **polecenie, int n, int k, char **argv){
        
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
    while(tmp!=NULL){
//        printf("TMP: %s", tmp);
        zamien_argumenty(argv,&tmp);
        ustaw_zmienna(tmp);
//        printf("/// %s || ", tmp);
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
    int fg_pid;
    
    wypisz_polecenie(&*i); 
        
    if ((fg_pid=fork())==0){
        signal(SIGINT, SIG_DFL);
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
//            printf("JEST DOBRZE\n");
//            execvp(i[0], i);
            wykonaj(i);
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
            pid = fg_pid;
//            printf("%d   \n\n", pid);
            pid_tab[k]=pid;
            signal(SIGINT,przekaz_sygnal);
            signal(SIGTSTP,przekaz_sygnal);
            int status;
//            printf("%d   ", n);
            if (n==1){
                waitpid(pid, &status, WUNTRACED | WCONTINUED);
                int j;
                for(j=0; j<k; j++){
                    kill(pid_tab[j],SIGINT);
                }
            }
//            pid = 0;
            signal(SIGTSTP, SIG_IGN);
//            printf("PID: %d | STATUS ZAKOŃCZENIA: %d\n", pid, status);             
        }
        if (n==1){
                waitpid(pid, NULL, WUNTRACED | WCONTINUED | WNOHANG);
                int j;
                for(j=0; j<k; j++){
//                    printf("KILL!");
                    kill(pid_tab[j], SIGINT);
                }
            }
    }

    return 0;    
}



char **pobierz_polecenie(char *linia, int *n, char **argv){
    char **start;    
    char **argumenty;
    argumenty = calloc(sizeof(char*), 128);
    start = argumenty;
    char *tmp;
    int i=0;
    
    tmp = strtok(linia, "|");
    while(tmp){
//        printf("%s  ", tmp);
//        if(strncmp(tmp,"$",1)==0){
//            memmove(tmp[1],tmp[0],strlen(tmp));
//            printf("%s ", tmp);
//            strcpy(argv[atoi(tmp)],tmp);
//        }
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

void zamien_argumenty(char **argv, char **linia){
    char *i;
    char *tmp;
    i = *linia;
    if (strchr(i,'$')!=NULL){
        tmp=malloc(strlen(strchr(i,'$'))*sizeof(char));
        strcpy(tmp,strchr(i,'$'));
        memmove(tmp,tmp+1,strlen(tmp));
        char *tmp2;
        int n;
        n = strlen(i) - strlen(tmp) -1;
//        printf("|| %s | |",tmp);
        if(isdigit(*tmp)){
            tmp2 = malloc(sizeof(char) * (n + strlen(argv[atoi(tmp)])));
            strncpy(tmp2,i,n);
            strcpy(tmp2+n,argv[atoi(tmp)]);
        } else{
            tmp2 = malloc(sizeof(char) * (n + strlen(getenv(tmp))));
            strncpy(tmp2,i,n);
            strcpy(tmp2+n,getenv(tmp));
            
        }
//        printf("|WYNIK: %s|",tmp2);
//        linia = malloc(strlen(tmp2) * sizeof(char));
//        strcpy(linia,tmp2);
//        tmp = *linia;
        *linia = tmp2;
        free(tmp);
//        free(tmp2);
        }   
}

void ustaw_zmienna(char *komenda){
    char *tmp;
    if(strchr(komenda, '=')){
    tmp = strtok(komenda,"=");
        if (tmp!=NULL){   
            setenv(tmp,strtok(NULL,"="),1);
        }
    }
    
}

int main(int args, char** argv) {

    char *linia;
    char *zacheta;
    char **polecenie;
    int pid;
    int i;
    int n;
    w_tle = 0;
    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    
    
    if(args > 1){
        i = open(argv[1], O_RDONLY, 0644);
        close(0);
        dup(i); 
//        close(1);
    }
       
    if(isatty(0)){
        strcpy(zacheta,"~~~>");  
    }
    else{
        strcpy(zacheta,"");  
    }


    while(linia = readline(zacheta)){
        
        add_history(linia);
        while(waitpid(-1,NULL,WNOHANG)>0);
        
        if (strncmp(linia,"#!",2)){
            polecenie = pobierz_polecenie(linia, &n, argv);
            for(i=0; i<n; i++){
                pipe(fd[i]);
            }
            i = 0;
    //            printf("KOMENDY: %d\n", n);
            while(*polecenie != NULL){
//                zamien_argumenty(argv, *polecenie);
    //            printf("%s\n", *polecenie++);
                wykonaj_polecenie(polecenie++, n--, i++,argv); 
            }
        }
    }
    
    return (EXIT_SUCCESS);
}