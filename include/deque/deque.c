#include "deque.h"
#include <stdio.h>
#include <stdlib.h>

int dq_get_total(struct Deque* deque)
{
    if(deque->first == deque->last)
       return 0;

    if(deque->first < deque->last)
        return deque->last - deque->first;
    else
        return deque->first - deque->last;
}

void* dq_dequeue(struct Deque* deque)
{
    if(dq_get_total(deque) <= 0)
        return 0;

    void* head = deque->buffer[deque->first]; 
    deque->buffer[deque->first] = 0; 
    deque->first++;
    return head;
}

void dq_enqueue(struct Deque* deque, void* value)
{
    if(dq_get_total(deque) == deque->size)
    {
        printf("Warning: Deque is full\n");
        return;
    }

    deque->buffer[deque->last] = value;
    deque->last++;
}

void* dq_pop_tail(struct Deque* deque)
{
    if(dq_get_total(deque) <= 0)
        return 0;
    deque->last--;

    void* tail = deque->buffer[deque->last];
    deque->buffer[deque->last] = 0;

    return tail;
}


void dq_init(struct Deque* deque, int size)
{
    dq_default(deque);
    deque->buffer = (void*)malloc(size * sizeof(void*));
    deque->size = size;
    for(int i = 0; i < deque->size; i++)
        deque->buffer[i] = 0;
}



void dq_default(struct Deque* deque)
{
    deque->buffer = 0;
    deque->size = 0;
    deque->first = 0;
    deque->last = 0;
}

void dq_release(struct Deque* deque)
{
    free(deque->buffer);
    dq_default(deque);
}

