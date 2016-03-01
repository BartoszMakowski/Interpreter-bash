/*
 * Interpreter polecen bash-a - projekt zaliczeniowy.
 */

/* 
 * File:   main.c
 * Author: bartosz
 *
 * Created on 18 stycznia 2016, 20:47
 */

#define N 100

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

#include "obrobka_tekstu.h"

/*
 * 
 */

int pid;
int fd[100][2];
//int pid_tab[N];
int jobs[N];

void przekaz_sygnal(int signo){
//    printf("Przesyłanie sygnału >>%i<< do procesu: >>PID: %i<<\n", signo, pid);
    printf("%d\n", signo);
    if(pid){
        if(signo == SIGTSTP){
            kill(pid, SIGSTOP);
            dodaj_do_jobs(pid);
            moje_fg(pid);
        }
        else{
            kill(pid, signo);
        }
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

void dodaj_do_jobs(int pid){
    int i;
    for(i=0; i<N; i++){
        if(!jobs[i]){
            jobs[i]=pid;
            break;
        }
    }
}

void wypisz_jobs(){
    int i;
    int s;
    printf("PROCESY W(S)TRZYMANE I (Z)AKOŃCZONE:\n");
    for(i=0; i<N; i++){
        if (jobs[i]){
            printf("PID: %d\n",jobs[i]);
//            s=waitpid(jobs[i], &s, WUNTRACED | WNOHANG | WCONTINUED);
//            if(WIFSTOPPED(s)) printf("S\t%d\n", jobs[i]);
//            else if (WIFEXITED(s)){
//                printf("Z\t%d\n", jobs[i]);
//                jobs[i]=0;
//            }

        }
    }
    


}

void moje_fg(int pid2){
    pid = pid2;
    kill(pid, SIGCONT);
    printf("PID %d", pid);
    int s;
    do{
        waitpid(pid, &s, WNOHANG);
    } while (!WIFEXITED(s) && !WIFSIGNALED(s)); 
    
    if (WIFEXITED(s)) {
        printf("zakonczony", WEXITSTATUS(s));
    } else if (WIFSIGNALED(s)) {
        printf("unicestwiony", WTERMSIG(s));
    } else if (WIFSTOPPED(s)) {
        printf("wstrzymany", WSTOPSIG(s));
    }//while(!WIFEXITED(s));

}

void wypisz_polecenie(char **polecenie){
    printf("POLECENIE:");
    while(*polecenie){
        printf(" %s",*polecenie++);
    }   
    printf("\n");    
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
    } else
    {
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
    char *tmp, *tmp2;
    
    ustaw_zmienna(*polecenie);
    
    tmp = strtok(*polecenie, " ");
    while(tmp!=NULL){
        zamien_argumenty(argv,&tmp);        
        (*i) = calloc(sizeof(char), strlen(tmp));
        strcpy(*i++, tmp);
        tmp = strtok(NULL," ");
    }
    
    i = start;
    
    if(strcmp(*i,"exit") == 0){
        exit(0);
    } else if(strcmp(*i,"fg") == 0){
            moje_fg(atoi(*++i));
            return;
    } else if(strcmp(*i, "jobs") == 0){
        wypisz_jobs();
        return;
    } else if(strcmp(*i, "unset") == 0){
        if(*++i){
            printf("NAZWA ZMIENNEJ: |%s|",*i);
            unsetenv(*i);
            return;
        }
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
    
        
    if ((fg_pid=fork())==0){
        signal(SIGINT, SIG_DFL);
            if(k > 0){
                dup2(fd[k-1][0], 0);
                close(fd[k-1][1]);
            }
            if(n>1){
                dup2(fd[k][1], 1);
                close(fd[k][0]);
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
            fd_we = open(wejscie, O_RDONLY, 0644);
            dup2(fd_we,0);
        }
        
        if(!tlo){
            wykonaj(i);
        }
        else{
            int pid_tlo;
            int status;
            pid_tlo = fork();
            if(pid_tlo == 0){
                wykonaj(i);                
            }
            else{
//                dodaj_do_jobs(pid_tlo);
                waitpid(pid_tlo, &status, 0);
                printf("PID: %d | STATUS ZAKOŃCZENIA: %d\n", pid_tlo, status); 
                exit(0);
            }
        }

    }
    else{
        if(k>0){
            close(fd[k-1][0]);
        }
        if(n>1){
            close(fd[k][1]);
        }
        else{
//            waitpid(fg_pid, NULL, 0);
        }
    }

    return fg_pid;
}



char **pobierz_polecenie(char *linia, int *n, char **argv, int *tlo){
    char **start;    
    char **argumenty;
    argumenty = calloc(sizeof(char*), 128);
    start = argumenty;
    char *tmp;
    int i=0;    
    tmp = linia;
    
    *tlo = 0;
    while(*tmp)
    {
        if(*tmp=='&'){
            *tlo = 1;
            break;
        }
        tmp++;        
    }
    
    tmp = strtok(linia, "|");
    while(tmp){
        (*argumenty) = calloc(sizeof(char), strlen(tmp));
        strcpy(*argumenty++, tmp);
        tmp = strtok(NULL, "|");
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
        if(isdigit(*tmp)){
            tmp2 = malloc(sizeof(char) * (n + strlen(argv[atoi(tmp)])));
            strncpy(tmp2,i,n);
            strcpy(tmp2+n,argv[atoi(tmp)]);
        } else{
            
            if(getenv(tmp)){
                tmp2 = malloc(sizeof(char) * (n + strlen(getenv(tmp))));
                strncpy(tmp2,i,n);
                strcpy(tmp2+n,getenv(tmp));
            }
            else{
                tmp2 = malloc(sizeof(char) * n);
                strncpy(tmp2,i,n);                
            }
        }
        *linia = tmp2;
        free(tmp);
    }   
}

void ustaw_zmienna(char *komenda){
    char *tmp, *tmp2;
    if(strchr(komenda, '=')){
    tmp = strtok(komenda,"=");
        if (tmp!=NULL){   
            tmp2=strtok(NULL,"=");
            if(strchr(tmp2,'"')){
                tmp2=strtok(tmp2,"\"");
            }else{
                tmp2=strtok(tmp2," ");                
            }
            setenv(tmp,tmp2,1);
        }
    }
    
}

void wykonaj_z_potokami(char **polecenie, int n, int i, char **argv){
    
    if(*polecenie != NULL){  
        wykonaj_z_potokami(polecenie+1, n-1, i+1, argv);
        pipe(fd[i-1]);
        wykonaj_polecenie(polecenie, n, i, argv);
    }    
}

int main(int args, char** argv) {

    char *linia;
    char *zacheta;
    char **polecenie;
    int pid = 0;
    int i;
    int n;
    int *w_tle;
    
    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGCLD, SIG_IGN);
    
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
                    wykonaj_z_potokami(polecenie, n, 0, argv);
                    exit(0);
                }
                waitpid(pid, NULL, 0);
                pid = 0;
            }
            else if (n == 1) {                
                int tmp_pid;
                tmp_pid=wykonaj_polecenie(polecenie, 1, 0, argv);
                if(*w_tle == 0){                    
                    pid=tmp_pid;
                    waitpid(pid, 0, NULL);
                    pid = 0;
                }
            }             
        }
        free(linia);
    }
    
    return (EXIT_SUCCESS);
}
