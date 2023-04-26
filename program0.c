// Zachary Wenzel
// Use -pthread flag to compile
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>
#include <time.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

void main(int argc, char * argv[]) {
    if (argc == 3) {
        int PID1, PID2, PID3;
        int exitCode1;
        int pfd1[2];
        int pfd2[2];
        char fdstr[10];
        char argstr[10];
        char semaphore[10];
        char semaphore2[10];
        char memid[10];

        const char *semName = "semaphore";
        const char *semName2 = "semaphore2";

        sem_t *sem_id = sem_open(semName, O_CREAT, 0777, 1);
        sem_t *sem_id2 = sem_open(semName2, O_CREAT, 0777, 1);

        // Create memory
        key_t key = ftok("shmfile", 65);
        int shmid = shmget(key,1024, 0666|IPC_CREAT);
        char *shmem = (char*) shmat(shmid, (void*)0, 0);


        if (sem_id == SEM_FAILED || sem_id2 == SEM_FAILED) {
            printf("failed to create sem\n");
        }

        pipe(pfd1);
        pipe(pfd2);

        if(fcntl(pfd1[0], F_SETFL, O_NONBLOCK) < 0){
            exit(2);
        }
        if(fcntl(pfd2[0], F_SETFL, O_NONBLOCK) < 0){
            exit(2);
        }

        PID1 = fork();
        if (PID1 == 0) {
            sprintf(fdstr, "%d", pfd1[1]);
            execl("program1", fdstr, semName, argv[1], (char *) 0, NULL);
        }

        PID2 = fork();
        if (PID2 == 0) {
            sprintf(fdstr, "%d", pfd1[0]);
            sprintf(argstr, "%d", pfd2[1]);
            sprintf(memid, "%d", shmid);
            execl("program2", fdstr, semName, argstr, semName2, memid, (char *) 0, NULL);
        }

        PID3 = fork();
        if (PID3 == 0) {
            sprintf(fdstr, "%d", pfd2[0]);
            sprintf(memid, "%d", shmid);
            execl("program3", fdstr, semName2, argv[2], memid, (char *) 0, NULL);
        }

        if (PID1 != 0 || PID2 != 0 || PID3 != 0) {
            wait(&exitCode1);
        }
        sleep(7);
        sem_close(sem_id);
        sem_unlink(semName);
        sem_close(sem_id2);
        sem_unlink(semName2);

        shmdt(shmem);
        shmctl(shmid, IPC_RMID, NULL);
    } else {
        printf("Two arguments are required.\n");
    }
}