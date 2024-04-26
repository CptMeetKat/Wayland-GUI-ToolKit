#include "stack.h"
#include <stdlib.h>

void stack_release(struct Stack* stack)
{
    free(stack->buffer);
    stack->buffer = 0;
}

void stack_default(struct Stack* stack)
{
    stack->size = 0;
    stack->buffer = 0;
    stack->capacity = 0;
}


void stack_init(struct Stack* stack, int capacity)
{
    stack_default(stack);
    stack->buffer = malloc(capacity * sizeof(void*));
    stack->capacity = capacity;
}


void* stack_pop(struct Stack* stack)
{
    if(stack->size <= 0)
        return 0;

    void* result = stack->buffer[stack->size - 1];
    stack->buffer[stack->size - 1] = 0;
    stack->size--;
    return result;
}

int stack_push(struct Stack* stack, void* elt)
{
    if(stack->size >= stack->capacity)
        return 0;


    stack->buffer[stack->size] = elt;
    stack->size++;
    return 1;
}
