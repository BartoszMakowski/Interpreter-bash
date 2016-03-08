#include "moje_jobs.h"

void dodaj_do_jobs(int pid, int *jobs, int n){
    int i;
    for(i=0; i<n; i++){
        if(!jobs[i]){
            jobs[i]=pid;
            break;
        }
    }
}

void usun_z_jobs(int pid, int *jobs, int n){
	int i;
    for(i=0; i<n; i++){
        if(jobs[i] == pid){
            jobs[i]=0;
            break;
        }
    }
}

void wypisz_jobs(int *jobs, int n){
	printf("N: %d\n", n);
    int i;
    printf("PROCESY WSTRZYMANE:\n");
    for(i=0; i<n; i++){
        if (jobs[i]){
            printf("PID: %d\n",jobs[i]);
        }
     }
    
    printf("\n");
    return ;
}

void moje_fg(int *pid, int *jobs, int n){    
    kill(*pid, SIGCONT);
    //printf("PID %d", *pid);
    int s;
    s=-1;
    while(1){
        waitpid(*pid, &s, WUNTRACED | WCONTINUED);
    
        if (WIFEXITED(s)) {
            printf("zakonczony\n", WEXITSTATUS(s));
            usun_z_jobs(*pid, jobs, n);
            break;
        } else if (WIFSIGNALED(s)) {
            printf("unicestwiony\n", WTERMSIG(s));
            usun_z_jobs(*pid, jobs, n);
            break;
        } else if (WIFSTOPPED(s)) {
            printf("wstrzymany\n", WSTOPSIG(s));
            break;
        }        
    }
    
    *pid = 0;
    return;

}

