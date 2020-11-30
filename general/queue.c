#include <stdio.h>
#include <stdlib.h>

struct node
{
    char *path;
    struct node *next;
};

struct node *front = NULL;
struct node *rear = NULL;
int size = 0;
int isQueueEmpty()
{
    if (front == NULL)
        return 1;
    else
        return 0;
}

void enqueue(char *path)
{
    struct node *new;
    new = malloc(sizeof(struct node));
    new->path = path;
    new->next = NULL;
    if (isQueueEmpty())
    {
        front = new;
        rear = new;
    }

    else
    {
        rear->next = new;
        rear = new;
    }
    size++;
}

char *dequeue()
{
    if (!isQueueEmpty())
    {
        char *path = front->path;
        struct node *erase = front;
        if (front == rear)
        {
            front = NULL;
            rear = NULL;
        }
        else
        {
            front = front->next;
        }

        free(erase);
        size--;
        return path;
    }

    else
    {
        return "";
    }
}

void printQueue()
{
    printf("Queue size is %d\n", size);
    struct node *temp = front;
    while (temp != NULL)
    {
        printf("%s\n", temp->path);
        temp = temp->next;
    }
}

void clearQueue()
{
    struct node *temp = front;
    struct node *erase;
    while (temp != NULL)
    {
        erase = temp;
        temp = temp->next;
        free(erase);
    }
}
