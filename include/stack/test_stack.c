

#include "stack.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


void release_test_data(struct Stack* stack)
{
    while(stack->size > 0)
    {
        int* elt = stack_pop(stack);
        free(elt);
    }
}

void add(struct Stack* stack, int value)
{
    int* i = malloc(sizeof(int));
    *i = value;
    if(! stack_push(stack, i))
        free(i);
}

int stack_test_buffer_equality(struct Stack* result, int* expected, int expected_length)
{
    //dq_total_tests++;

    int dq_length = result->size;
    if( expected_length != dq_length)
        return 0;


    for(int i = 0; i < dq_length; i++)
    {
        int* value = result->buffer[i]; 
        if(*value != expected[i] )
            return 0;
    } 

    return 1;
}

int test_push()
{
    int expected[] = {5,7,9};
    int expected_length = 3;
    
    struct Stack s;
    stack_init(&s, 3);

    add(&s, 5);
    add(&s, 7);
    add(&s, 9);
    add(&s, 11);
    
    int result = stack_test_buffer_equality(&s, expected, expected_length);
    
    assert(result);
    printf("--Push--\n");
    printf("* Stack does not exceed capacity when over inserted\n");
    printf("* Stack contains inserted values\n");
    printf("* Stack is correct size\n");

    release_test_data(&s);
    stack_release(&s);

    return result;
}


int test_pop()
{
    int expected[] = {9,7,5};
    int expected_length = 3;
    int result = 1;
    struct Stack s;
    stack_init(&s, 3);

    add(&s, 5);
    add(&s, 7);
    add(&s, 9);


    int* value = stack_pop(&s);
    int i = 0;
    while(value != 0)
    {
        if(*value != expected[i++])
            result = 0;
        free(value);
        value = stack_pop(&s);
    }
    
    assert(result);
    printf("--Pull--\n");
    printf("* Stack does not return data when empty\n");
    printf("* Stack contains all inserted values\n");

    release_test_data(&s);
    stack_release(&s);

    return result;
}

int main()
{
    test_push();
    printf("\n");
    test_pop();
}
