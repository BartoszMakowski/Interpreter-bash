#include <stdio.h>
#include "obrobka_tekstu.h"

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
