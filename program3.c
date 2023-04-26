// Zachary Wenzel
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/shm.h>

void main(int argc, char * argv[]) {
    FILE *outputFile;
    int pfd2[2];
    pfd2[0] = atoi(argv[0]);

    sem_t *sem2 = sem_open(argv[1], O_CREAT, 0777, 1);
    if (sem2 == SEM_FAILED) {
        printf("failed to create sem, program 3\n");
    }
    char word[100];

    outputFile = fopen(argv[2], "w");
    int i = 0; 
    int val;
    while(1) {
        sem_wait(sem2);
        val = read(pfd2[0], word, 100);
        sem_post(sem2);
        if (val <= 0) {
            usleep(1000);
            continue;
        }
        if (strcmp(word, "xx0") == 0) {
            break;
        }
        if (word[strlen(word) - 1] == '.') {
            fprintf(outputFile, "%s\n", word);
        } else { 
            fprintf(outputFile, "%s ", word);
        }
        
        
        i++;
    }
    fclose(outputFile);
    close(pfd2[0]);
    sleep(1);

    char *shmem = (char*) shmat(atoi(argv[3]), (void*)0, 0);
    printf("%s", shmem);

    shmdt(shmem);
}