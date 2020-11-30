
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "test.h"
#include <string.h>
#include <stdlib.h>
#include "../general/queue.c"
#include "../general/process.c"

void handleContinuesSignal(int sig)
{
    printf("Process %d resume\n", getpid());
}

int main(int argc, char const *argv[])
{
    enqueue("Hola");
    enqueue("Hola 1");
    enqueue("Hola 2");
    enqueue("Hola 3");
    enqueue("Hola 4");
    signal(SIGCONT, handleContinuesSignal);
    int status, procn, PROC = 1, last = 0;
    int fd[PROC][2];
    struct process availableProc[PROC];

    for (procn = 0; procn < PROC; procn++)
    {
        availableProc[procn].pid = fork();
        availableProc[procn].state = 1;
        pipe(fd[procn]);
        if (availableProc[procn].pid == 0)
        {
            for (int j = 0; j < procn; j++)
            {
                close(fd[j][0]);
                close(fd[j][1]);
            }
            close(fd[procn][0]);
            break;
        }
    }

    if (availableProc[procn].pid == 0)
    {
        pause();
        wait(NULL);
        close(fd[procn][1]);
        char arr[50];
        read(fd[procn][0], arr, 50);
        printf("Received in process %d was %s\n", getpid(), arr);
    }
    else
    {
        printf("Antes del while\n");
        while (1)
        {
            printf("Dentro del while\n");
            if (!isQueueEmpty())
            {
                for (int i = 0; i < PROC; i++)
                {
                    if (!isQueueEmpty())
                    {
                        char send[50];
                        strcpy(send, dequeue());
                        close(fd[i][0]);
                        write(fd[i][1], send, 50);
                        kill(availableProc[i].pid, SIGCONT);
                    }
                }
            }
            if (last != sizeQueue)
            {
                last = sizeQueue;
                printf("%d\n", last);
            }
        }
    }
    printf("Exit from parent\n");

    return 0;
}
