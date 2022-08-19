#include <stdio.h>

#define queue_size      50
#define mesg_size       15

typedef struct queue_mesg{
    char mesg[queue_size][mesg_size];
    int front;
    int rear;
}queue_mesg;

void queue_clear(queue_mesg *queue);
int queue_insert(queue_mesg *queue, char *mesg);
int queue_get(queue_mesg *queue, char *mesg);
