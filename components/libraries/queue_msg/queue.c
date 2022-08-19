#include <stdio.h>
#include <string.h>
#include <queue.h>

//----------------- Message Queue -----------------
void queue_clear(queue_mesg *queue){
    queue->front = 0;
    queue->rear = 0;
    memset(queue->mesg, 0, sizeof(queue->mesg));
}

int queue_insert(queue_mesg *queue, char *mesg){
    if((queue->rear - queue->front) == queue_size)
        return 1;
    memcpy(queue->mesg[queue->rear++ % queue_size], mesg, mesg_size);
    return 0;
}

int queue_get(queue_mesg *queue, char *mesg){
    if(queue->rear == queue->front)
        return 1;
    memcpy(mesg, queue->mesg[queue->front++], mesg_size);
    if(queue->front == queue_size){
        queue->front = 0;
        queue->rear %= queue_size;
    }
    return 0;
}
//-------------------------------------------------
