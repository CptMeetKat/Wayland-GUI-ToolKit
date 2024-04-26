

#ifndef STACK_H
#define STACK_H

struct Stack
{
    int capacity;
    int size;
    void** buffer;
};

void stack_release(struct Stack* stack);
void stack_init(struct Stack* stack, int capacity);
void* stack_pop(struct Stack* stack);
int stack_push(struct Stack* stack, void* elt);

#endif


