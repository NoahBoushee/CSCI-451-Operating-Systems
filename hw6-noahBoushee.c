/*
CSCI 451: Homework 6 Noah Boushee
A 5 threaded program that uses mutexs to avoid the lost update problem in
a bank account situation. Create a least recently used page algorithm to the above
to manage it

gcc -pthread -o hw6 hw6-noahBoushee.c
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
    int id;
    int numPages;

};

struct node{
    int key;
    int inUse;
    int init;
    int owner;
    int R;
    int M;
    struct node *next;
};

struct node *head = NULL;
//struct node *current = NULL;

void insert(int key){
    struct node *link = (struct node*) malloc(sizeof(struct node));
    link->key = key;
    link->inUse = 0;
    link->init = 1;
    link->R = 0;
    link->M = 0;
    link->owner = -1;

    if (head == NULL){
        head = link;
        head->next = head;
    }
    else{
        link->next = head;
        head = link;
    }

}

void freeLL(int numPages){
    struct node *temp = head;
    for (int i = 0; i < numPages-1; i++){
        temp = temp->next;
        free(temp);
    }
    free (head);
}

void resetAlogirthm(int numPages){
    struct node *temp = head;
    for (int i = 0; i < numPages; i++){
        temp->R = 0;
        //printf("RESET NODE %d\n", temp->key);
        temp = temp->next;
    }
    printf("OS RESET ALL R BITS\n");
}
//ADD CHECK FOR IF PAGE IS UNITIALIZED
void pageFault(int threadID, int numPages){
    int currentLowestFault = 0;
    int R = 0, M = 0;
    int flag = 0;
    struct node *temp = head;
    for (int i = 0; i < numPages; i++){
        if (temp->owner != threadID && flag == 0){
            if(temp->owner == -1){
                currentLowestFault = i;
                flag = 1;
                //printf("Thread %d replace thread NONE with R bit %d and M bit %d\n", threadID, temp->R, temp->M);
                //temp->owner = threadID;
            }
            
            else if (temp->R == 0 && temp->M == 0 && temp->init > 2){
                currentLowestFault = i;
                flag = 1;
            }
            else if (temp->R == 0 && temp->M == 1){
                R = 1;
                M = 0;
                temp->init = 0;
                currentLowestFault = i;
            }
            else if (temp->R == 1 && temp->M == 0 && R != 0 && M != 1){
                R = 1;
                M = 0;
                temp->init += 1;
                currentLowestFault = i;  
            }
            else if (temp->init == 0){
                temp->init +=1;
                currentLowestFault = i;  
            }
        }
        temp->init += 1;
        temp = temp->next;
    }
    temp = head;
    //Get extra page if we didn't get one just choose page two could do rn but this is fine
    if(currentLowestFault != 0){
        for (int i = 0; i < numPages; i++){
            if (temp->key == i && temp->owner != threadID){
                 printf("Thread %d replace thread %d with R bit %d and M bit %d INIT %d\n", threadID, temp->owner, temp->R, temp->M, temp->init);
                temp->owner = threadID;
                temp->init = 0;
                temp->R = 0;
                temp->M = 0;
            }
            temp = temp->next;
        } 
           //printf("IN THE IF\n");
    }
    else{
        int tempFlag = 0;
        for (int i = 0; i < numPages; i++){
            if (temp->owner == threadID){
                printf (" IN THE Thread %d replace thread %d", threadID, temp->owner);
                temp = temp->next;
            }
            else{
                //printf("IN THE ELSE\n");
                printf("Thread %d replace thread %d with R bit %d and M bit %d INIT %d\n", threadID, temp->owner, temp->R, temp->M, temp->init);
                temp->owner = threadID;
                temp->init = 0;
                temp->R = 0;
                temp->M = 0;
                tempFlag = 1;
            break;
            }
        }
        if(tempFlag == 0){printf("ALL PAGES ALREADY BELONG TO THREAD %d\n", threadID);}
    }
    
}

void *threads(void *args){
    struct threadArgs *args2 = (struct threadArgs *) args;
    //printf("ENTERED THREAD %d\n", (args2->id));
    FILE *fp;
    char file[12];
    snprintf(file, 12, "./data%d.in", (args2->id));
    fp = fopen(file, "r");
    if (fp == NULL){
       exit(EXIT_FAILURE);
    }
    //printf("THREAD %d OPENED %s\n", (args2->id), file);
    struct node *temp = head;
    int flag = 0;
    for (int i = 0; i < args2->numPages; i++){
        if (temp->inUse == 0 && flag == 0){
            temp->inUse = 1;
            temp->owner = args2->id;
            flag = 1;
            break;
        }
        temp = temp->next;
    }
    char buffer[1026];
    float tempBalance = 0;
    srand(time(NULL));
    int randomSleepTime = (rand() % (10000 - 5000+1) + 5000);
    int randomResetErrorTime;
    
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
            //The below if statement is used to simulate random page fault and random reset of bits
            randomResetErrorTime = (rand() % (100 - 0+1) + 0);
            if (randomResetErrorTime > 97){
                resetAlogirthm(args2->numPages);
            }
            
           
            int hasPage = 0, positiveVal = 1;
            temp = head;
            //Below code checks if account balance is + or - changes bits depending on case
            if (accountVal < 0){
                positiveVal = 0;
            }
            flag = 0;
            for (int i = 0; i < args2->numPages; i++){
                if (temp->owner == args2->id && flag == 0){
                    if(positiveVal == 0){
                        temp->R = 1;
                        temp->M = 1;
                    }
                    else{
                        temp->R = 1;
                    }
                    hasPage = 1;
                    flag = 1;
                    break;
                }
                temp = temp->next;
            }
            if (flag == 0){
                //GENERATE PAGE FAULT THREAD HAS NO PAGE
                printf("PAGE FAULT IN THREAD %d THREAD HAS NO PAGE\n", args2->id);
                pageFault(args2->id, args2->numPages);
                
            }
            else if(randomResetErrorTime < 15){
                printf("PAGE FAULT IN THREAD %d RANDOM PAGE FAULT\n", args2->id);
                pageFault(args2->id, args2->numPages);
                //CALL PAGE FAULT FUNCTION
            }
            accountVal += tempBalance;
            tempBalance = 0;
             printf("ACCOUNT BALANCE AFTER THREAD %d is $%.2f\n", args2->id, accountVal);
            usleep(randomSleepTime);
            //printf("THREAD %d LEAVING CRITICAL SECTION\n", (args2->id));
            pthread_mutex_unlock(&lock);
        }
    }
    //LAST THREAD TO EXIT FREES LINKED LIST NEEDS MUTEX TO SYNC HERE SO 
    //WE DONT GET DOUBLE FREE SEG FAULT
    pthread_mutex_lock(&lock);
    int countNotUsThreads = 0;
    for (int i = 0; i < args2->numPages; i++){
        if (temp->owner == args2->id){
            temp->owner = -1;
            
        }
        else{countNotUsThreads += 1;}
        temp = temp->next;
    }
    if (countNotUsThreads > 0){
        printf("LAST THREAD WITH ID: %d FREEING LINKED LIST\n", args2->id);
        freeLL(args2->numPages);
    }
    pthread_mutex_unlock(&lock);
    printf("THREAD %d FINISHED AND FREED USED PAGES\n", (args2->id));
    fclose(fp);
    int exitCode = 1;
    pthread_exit(&exitCode);
}

void main(){
    int numThreads = 5;
    int numPages = 6;
    pthread_mutex_init(&lock, NULL);

    struct threadArgs *arrayArgs[numThreads];
    pthread_t arrayThreads[numThreads];

    for(int i = 0; i < numPages; i++){
        //printf("CREATED NODE %d\n", i+1);
        insert(i+1);
    }
    printf("NOTE: unitialized pages have the ID of -1\n");
    for (int i = 0; i < numThreads; i++){
        arrayArgs[i] = malloc(sizeof (struct threadArgs));
        arrayArgs[i]->id = i+1;
        arrayArgs[i]->numPages = numPages;
        pthread_create(&arrayThreads[i], NULL, threads, (void *)arrayArgs[i]);
        pthread_detach(arrayThreads[i]);
        //printf("CREATE AND DETACHED THREAD %d\n", i+1);
    }
    
    //usleep(50000);
    pthread_exit(NULL);
}
