/*
CSCI 451: Homework 3 Noah Boushee
Create a program that downloads a webpage and then parses it for instances of
two specific words, uses threads to do this.

gcc -pthread -o hw3 hw3-noahBoushee.c
*/
#include <sys/wait.h>
#include <sys/utsname.h>
#include <sched.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>

struct threadArgs{
    char *string2Find;
    char *list;

};

void *searchThread(void *args){
    struct threadArgs *args2 = (struct threadArgs *) args;
    int count = 0;
    while (args2->list = strstr(args2->list, args2->string2Find)){
        count++;
        args2->list++;
    }
    printf("TOTAL OCCURENCES OF %s IN FILE: %d FROM THREAD: %lu\n", args2->string2Find, count, pthread_self());

}

int main(){
    system("wget -q http://undcemcs01.und.edu/~ronald.marsh/CLASS/CS451/hw3-data.txt");
    FILE * fp;
    int size, i;
    //char buffer[bufferLen];
    char *fileArray;
    fp = fopen("hw3-data.txt", "r");
    if (fp == NULL){
        exit(EXIT_FAILURE);
    }
    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    //printf("%d\n SIZE", size);
    rewind(fp);
    fileArray = (char*)malloc(sizeof(char)*size+1);
    if(!fileArray){
        fclose(fp);
        printf("UNABLE TO ALLOCATE");
        exit(EXIT_FAILURE);
    }
    if(1 != fread(fileArray, size, 1, fp)){
        printf("UNABLE TO READ FILE");
        fclose(fp);
        free(fileArray);
        exit(EXIT_FAILURE);
    }
    for(i = 0; i < size; i++){
        fileArray[i] = tolower(fileArray[i]);
    }
    fclose(fp);  

    struct threadArgs *one = malloc(sizeof (struct threadArgs));
    struct threadArgs *two = malloc(sizeof (struct threadArgs));
    one->string2Find = "easy";
    two->string2Find = "polar";
    one->list = fileArray;
    two->list = fileArray;
    
    pthread_t tid;
    pthread_t tid2;
    pthread_create(&tid, NULL, searchThread, (void *)one);
    pthread_create(&tid2, NULL, searchThread, (void *)two);
    pthread_join(tid, NULL);
    pthread_join(tid2, NULL);
    //printf("%s", fileArray);
  
    
    free(one);
    free(two);
    free(fileArray);
    system("unlink hw3-data.txt");
    return 0;
}
