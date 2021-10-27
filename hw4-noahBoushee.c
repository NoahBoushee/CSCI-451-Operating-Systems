/*
CSCI 451: Homework 4 Noah Boushee
Create a program that reads the value of a file. The program creates two threads
one reads the value and writes it to a global variable, the other thread than
performs operations on it if it is odd. Main closes file once done. Semiphores and mutexs are used to syncrhonize threads.

gcc -pthread -o hw4 hw4-noahBoushee.c
*/
#include <sys/wait.h>
#include <sys/utsname.h>
#include <sched.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

int globalVal = NULL;
pthread_mutex_t lock;

struct threadArgs{
    FILE *fp;
    int *id;

};

void *threads(void *args){
    struct threadArgs *args2 = (struct threadArgs *) args;
    FILE *fp2;
    int size;
    if(args2->id == 0){
        fp2 = fopen("./hw4.in", "r");
        if (fp2 == NULL){
            exit(EXIT_FAILURE);
        }
    }
    while(1){
        if(args2->id ==0){
            //THREAD 1
            while(globalVal!=NULL){
                usleep(5);
            }
            pthread_mutex_lock(&lock);
            int temp = fscanf(fp2, "%d\n", &globalVal);
            if (globalVal == -1){globalVal = 1;}
            if (temp == -1){
                globalVal = EOF;
                pthread_mutex_unlock(&lock);
                break;
            }
            pthread_mutex_unlock(&lock);
            
        }
        else{
            //THREAD 2
            while(globalVal==NULL){
                usleep(5);
                if(globalVal==EOF){break;}
            }
            pthread_mutex_lock(&lock);
            if (globalVal == EOF){
                break;            
            }
            //usleep(5);
            if(globalVal % 2 == 0){
                fprintf(args2->fp, "%d\n", globalVal);
                fprintf(args2->fp, "%d\n", globalVal);
            }
            else{
                fprintf(args2->fp, "%d\n", globalVal);
            } 
           globalVal = NULL;  
           pthread_mutex_unlock(&lock);   
             
            
        }
    }
    if(args2->id == 0){
        
        fclose(fp2);
    }
    int exitCode = 1;
    pthread_exit(&exitCode);
}

int main(){
    FILE *fp;       
    fp = fopen("./hw4.out", "w");
    pthread_mutex_init(&lock, NULL);
    struct threadArgs *one = malloc(sizeof (struct threadArgs));
    struct threadArgs *two = malloc(sizeof (struct threadArgs));
    one->fp = fp;
    two->fp = fp;
    one->id = 0;
    two->id = 1;
    pthread_t tid;
    pthread_t tid2;
    pthread_create(&tid, NULL, threads, (void *)one);
    pthread_create(&tid2, NULL, threads, (void *)two);
    pthread_join(tid, NULL);
    pthread_join(tid2, NULL);

    pthread_mutex_destroy(&lock);
    free(one);
    free(two);     
    fclose(fp);
    return 0;
}
