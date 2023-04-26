// Zachary Wenzel
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include <fcntl.h>

void main(int argc, char * argv[]) {
    char string[100];
    int pfd[2];
    pfd[1] = atoi(argv[0]);
    sleep(1);
    sem_t *sem = sem_open(argv[1], O_CREAT, 0777, 1);
    if (sem == SEM_FAILED) {
        printf("failed to create sem, program 1\n");
    }
    int i = 0;
    
    FILE *file = fopen(argv[2], "r");
    while (!(feof(file))) {
        if (fscanf(file, "%s ", string) == 1) {
            sem_wait(sem);
            
            write(pfd[1], string, 100);
            sem_post(sem);
            usleep(10000);
        }
        ++i;
    }

    sem_wait(sem);
    write(pfd[1], "xx0", 100);
    close(pfd[1]);
    sem_post(sem);
    fclose(file);
}