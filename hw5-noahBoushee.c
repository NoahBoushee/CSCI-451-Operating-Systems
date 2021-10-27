/*
CSCI 451: Homework 5 Noah Boushee
A 5 threaded program that uses mutexs to avoid the lost update problem in
a bank account situation

gcc -pthread -o hw5 hw5-noahBoushee.c
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

float accountVal = 0;
pthread_mutex_t lock;

struct threadArgs{
    int *id;

};

void *threads(void *args){
    struct threadArgs *args2 = (struct threadArgs *) args;
    //printf("ENTERED THREAD %d\n", (args2->id));
    FILE *fp;
    char file[12];
    snprintf(file, 12, "./data%d.in", (args2->id));
    //printf("%s\n", file);
    fp = fopen(file, "r");
    if (fp == NULL){
       //printf("THREAD %d CANNOT OPEN FILE\n", (args2->id));
       exit(EXIT_FAILURE);
    }
    //printf("THREAD %d OPENED %s\n", (args2->id), file);
    
    char buffer[1026];
    float tempBalance = 0;
    srand(time(NULL));
    int randomSleepTime = (rand() % (1000000 - 500000+1) + 500000);
    while(fgets(buffer, sizeof(buffer), fp)){
        //printf("    THREAD: %d VALUE: %s\n", args2->id, buffer);
        usleep(randomSleepTime);
        if(strstr(buffer, "R")){
            //printf("THREAD %d IN RRRRR\n", (args2->id));
            pthread_mutex_lock(&lock);
            //printf("THREAD %d ENTERED CRITICAL SECTION\n", (args2->id));
            while(!strstr(buffer, "W")){
                fgets(buffer, sizeof(buffer),fp);
                tempBalance += atof(buffer);
            }
            accountVal += tempBalance;
            tempBalance = 0;
            printf("ACCOUNT BALANCE AFTER THREAD %d is $%.2f\n", args2->id, accountVal);
            usleep(randomSleepTime);
            //printf("THREAD %d LEAVING CRITICAL SECTION\n", (args2->id));
            pthread_mutex_unlock(&lock);
        }
    }
    //printf("THREAD %d DONE\n", (args2->id));
    fclose(fp);
    int exitCode = 1;
    pthread_exit(&exitCode);
}

void main(){
    int numThreads = 5;
    pthread_mutex_init(&lock, NULL);

    struct threadArgs *arrayArgs[numThreads];
    pthread_t arrayThreads[numThreads];

    for (int i = 0; i < numThreads; i++){
        arrayArgs[i] = malloc(sizeof (struct threadArgs));
        arrayArgs[i]->id = i+1;
        pthread_create(&arrayThreads[i], NULL, threads, (void *)arrayArgs[i]);
        pthread_detach(arrayThreads[i]);
        //printf("CREATE AND DETACHED THREAD %d\n", i+1);
    }
    
    //usleep(50000);
    //printf("FINAL ACCOUNT BALANCE is $%f\n", accountVal);
    pthread_exit(NULL);
}
