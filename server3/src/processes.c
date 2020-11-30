#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#define PROC 5
int main(int argc, char const *argv[])
{
    pid_t pid[PROC];
    if (fork() == 0)
    {
        printf("Hello world from Child %d %d\n", getpid(), getppid());
    }
    else
    {
        printf("Hello world from Parent\n");
    }

    return 0;
}
