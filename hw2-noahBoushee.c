/*
CSCI 451: Homework 2 Noah Boushee
Create a fork bomb using fork and firefox
*/
#define _GNU_SOURCE
       #include <sys/wait.h>
       #include <sys/utsname.h>
       #include <sched.h>
       #include <string.h>
       #include <stdint.h>
       #include <stdio.h>
       #include <stdlib.h>
       #include <unistd.h>
       #include <sys/mman.h>

#define STACK_SIZE 65536
int main(){
    int ExitCode = 1, a;
    pid_t PID, PID2;
    char *Command[] = {"firefox", NULL};
    char *stack;
    stack = malloc(STACK_SIZE);
    printf("Spawning a child.\n");
    printf("-----------------\n");
    PID = fork();
    if (PID == 0) {
       printf("CHILD: Hello from the child.\n");
       a = execv("/usr/bin/firefox", Command);
       printf("CHILD: exit code: %d\n", ExitCode);
       exit(ExitCode);
    } 
    else {
       printf("PARENT: Spawning child PID: %ld\n", PID);
       //execv("/usr/bin/firefox", Command);
       //system("ps -u root");
       waitpid(PID, &ExitCode, 0);
       PID2 = clone (main, stack + STACK_SIZE, CLONE_SIGHAND|CLONE_FS|CLONE_VM|CLONE_FILES, "");
       waitpid(PID2, &ExitCode, 0);
       printf("PARENT: Child exit code: %d\n", (ExitCode >> 8));
    }
    return 1;
}
