#include "deque.h"
#include <stdio.h>
#include <stdlib.h>

static int dq_mod(int value, int mod)
{//Does not work on big numbers yet (change to while)
    int result = value;
    if(value >= mod)
        result = value % mod;
    else if(value < 0)
    {
       result = value + mod;
    }
    return result;
}

int dq_get_total(struct Deque* deque)
{
//    if(deque->first == deque->last)
//       return 0;
//
//    if(deque->first < deque->last)
//        return deque->last + 1 - deque->first;
//    else
//        return deque->first - deque->last;
    return deque->total;
}

void* dq_dequeue(struct Deque* deque)
{
    if(dq_get_total(deque) <= 0)
        return 0;

    void* head = deque->buffer[deque->first]; 
    deque->buffer[deque->first] = 0;
    deque->first++;
    deque->first = dq_mod(deque->first, deque->size);

    deque->total--;

    return head;
}

int dq_enqueue(struct Deque* deque, void* value)
{
    if(dq_get_total(deque) >= deque->size)
    {
        printf("Warning: Deque is full\n");
        return 0;
    }

    deque->buffer[deque->last] = value;
    deque->last++;
    deque->last = dq_mod(deque->last, deque->size);

    deque->total++;
    return 1;
}

void* dq_pop_tail(struct Deque* deque)
{
    if(dq_get_total(deque) <= 0)
        return 0;
    deque->last--;
    deque->last = dq_mod(deque->last, deque->size);
    void* tail = deque->buffer[deque->last];
    deque->buffer[deque->last] = 0;

    deque->total--;

    return tail;
}


void dq_init(struct Deque* deque, int size)
{
    dq_default(deque);

    deque->buffer = malloc(size * sizeof(void*));
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
    deque->total = 0;
}

void dq_release(struct Deque* deque)
{
    free(deque->buffer);
    dq_default(deque);
}

