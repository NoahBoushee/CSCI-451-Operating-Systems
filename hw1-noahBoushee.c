/*
CSCI 451: Homework 1 Noah Boushee
Scrap a webpage using WGET. Parse the contents of this webpage and print selected contents to screen. Delete the downloaded files using UNLINK
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(){

    system("wget -q http://undcemcs01.und.edu/~ronald.marsh/PROJECTS/PROJECTS.HTML");
    
    FILE * fp;
    int bufferLen = 255; 
    char buffer[bufferLen];
    fp = fopen("PROJECTS.HTML", "r");
    if (fp == NULL){
        exit(EXIT_FAILURE);
    }
    printf("LIST OF PROJECTS:\n");
    while (fgets(buffer, bufferLen, fp)){
        if (strchr(buffer, '<') == NULL && buffer[0] != '\n' && strchr(buffer, '"') == NULL && buffer[0] == ' '){
            int count = 0;
            while(buffer[count] == ' '){
                count++;
            }
            //Always null terminated as strings will never be longer
            strncpy(buffer, &buffer[count], bufferLen);
            printf("%s", buffer);
        }
        
    }

    fclose(fp);
    system("unlink PROJECTS.HTML");
}
