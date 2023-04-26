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
    char word[100];
    char c;
    char endPunct;
    int uppercase_vowel, lowercase_vowel;
    int wordCount = 0;
    int pfd[2];
    int pfd2[2];
    pfd[0] = atoi(argv[0]);
    pfd2[1] = atoi(argv[2]);
    sem_t *sem = sem_open(argv[1], O_CREAT, 0777, 1);
    sem_t *sem2 = sem_open(argv[3], O_CREAT, 0777, 1);
    if (sem == SEM_FAILED || sem2 == SEM_FAILED) {
        printf("failed to create sem, program 2\n");
    }

    char *shmem = (char*) shmat(atoi(argv[4]), (void*)0, 0);

    int v;
    int i = 0;
    int endP = 0;
    int vowelWordCount = 0;
    int consonantWordCount = 0;
    while (1) {
        sem_wait(sem);
        v = read(pfd[0], word, 100);
        if (v <= 0) {
            sem_post(sem);
            usleep(10000);
            continue;
        }
        if (strcmp(word, "xx0") == 0) {
            sem_post(sem2);
            write(pfd2[1], word, 100);
            sem_post(sem2);
            break;
        }

        //Pig latin translate
        c = word[0];
        lowercase_vowel = (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u');
        uppercase_vowel = (c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U');
        if (uppercase_vowel || lowercase_vowel) {
            if(ispunct(word[strlen(word)-1])) {
                endPunct = word[strlen(word)-1];
                word[strlen(word)-1] = 'r';
                strncat(word, "ay", 3);
                strncat(word, &endPunct, 1);
            } else {
                strncat(word, "ray", 4);
            }
            vowelWordCount++;

        } else {
            int len = strlen(word);
            char firstWord = word[0];
            if(ispunct(word[strlen(word)-1])) {
                endPunct = word[strlen(word)-1];
                endP = 1;
                len--;
            }

            for (int i = 1; word[i] != '\0'; i++) {
                word[i-1] = word[i];
            }

            word[len-1] = firstWord;
            word[len] = '\0';
            strncat(word, "ay", 3);
            
            if (endP) {
                strncat(word, &endPunct, 1);
                endP = 0;
            }

            consonantWordCount++;
        }
        
        sem_wait(sem2);
        write(pfd2[1], word, 100);
        sem_post(sem2);
        
        wordCount++;
        i++;

        sem_post(sem);
    }

    char val[100];
    sprintf(val, "Type 1: %d\nType 2: %d\n", vowelWordCount, consonantWordCount);
    memmove(shmem, val, strlen(val)+1);

    shmdt(shmem);
    close(pfd[0]);
    close(pfd2[1]);
}