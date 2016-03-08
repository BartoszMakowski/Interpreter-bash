#include "polecenia.h"

void wykonaj(char **polecenie){
    if(strcmp(polecenie[0], "echo") == 0){
        char **i;
        i = polecenie;
        while(*++i){
            printf("%s ",*i);
        }
    } else {
        execvp(polecenie[0], polecenie);
    }
}

int wykonaj_polecenie(char **polecenie, int n, int k, char **argv){
        
    int tlo = 0;
    char **i = calloc(sizeof(char*), 128);
    char **start;    
    char dopisz = 0;
    char *wyjscie = NULL;
    char *wejscie = NULL;
    int fd_we, fd_wy; // deskeyptory WE/WY
    char *tmp;
    
    start = i;
    
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
        pid = atoi(*++i);
        signal(SIGTSTP, przekaz_sygnal);            
        moje_fg(&pid, jobs, N);
        return 0;
    } else if(strcmp(*i, "jobs") == 0){
        wypisz_jobs(jobs, N);
        return 0;
    } else if(strcmp(*i, "unset") == 0){
        if(*++i){
            //printf("NAZWA ZMIENNEJ: |%s|",*i);
            unsetenv(*i);
            return 0;
        }
    } 
    
    while (*i) {
        if(strcmp(*i,">")==0){
            wyjscie = *++i;
            czysc_polecenie((i-1));
            i--;
        } else if(strcmp(*i,">>")==0){
            dopisz = 1;
            wyjscie = *++i;
            czysc_polecenie((i-1));            
            i--;
        } else if(strcmp(*i++,"<")==0){
            wejscie = *i++;
            czysc_polecenie((i-2));
            i-=2;
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
        domyslne_sygnaly();
        signal(SIGTSTP, SIG_DFL);
            if(k > 0){
                dup2(fd[k-1][0], 0);
                close(fd[k-1][1]);
            }
            if(n>1){
                dup2(fd[k][1], 1);
                close(fd[k][0]);
            }
        if(wyjscie){
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
            signal(SIGTSTP, SIG_DFL);
            wykonaj(i);
        }
        else{
            int pid_tlo;
            int status;
            pid_tlo = fork();
            if(pid_tlo == 0){
                signal(SIGINT, SIG_IGN);
                wykonaj(i);                
            }
            else{
                signal(SIGCLD, SIG_DFL);
                signal(SIGINT, SIG_IGN);
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
    }
    free(i);
    return fg_pid;
}

void przekaz_sygnal(int signo){
    
    printf("%d\n", signo);
    if(pid){
        if(signo == SIGTSTP){
            //printf("Przesyłanie sygnału >>%i<< do procesu: >>PID: %i<<\n", signo, pid);
            kill(pid, SIGSTOP);
            dodaj_do_jobs(pid, jobs, N);
        }
        else{
            kill(pid, signo);
        }
    }
}

int wykonaj_z_potokami(char **polecenie, int n, int i, char **argv){
    int tmp_pid;
    
    if(*polecenie != NULL){
        wykonaj_z_potokami(polecenie+1, n-1, i+1, argv);
        pipe(fd[i-1]);        
        if(n==1){
            tmp_pid = wykonaj_polecenie(polecenie, n, i, argv);
        }
        else{
            wykonaj_polecenie(polecenie, n, i, argv);
        }
    }        
    return tmp_pid;
}

void domyslne_sygnaly(){
    signal(SIGCLD, SIG_DFL);
    signal(SIGINT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);
}

