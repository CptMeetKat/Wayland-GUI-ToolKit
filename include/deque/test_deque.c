
#include "deque.h"
#include <stdio.h>
#include <stdlib.h>


int dq_total_tests = 0;

int dq_test_buffer_equality(struct Deque* result, int* expected, int expected_length)
{
    dq_total_tests++;

    int dq_length = dq_get_total(result);
    if( expected_length != dq_length)
        return 0;


    for(int i = 0; i < dq_length; i++)
    {
        int position = (result->first + i) % result->size; 
        int* value = result->buffer[position]; 

        if(*value != expected[i] )
            return 0;
    } 

    return 1;
}


int dq_test_value_equality(int result, int expected)
{
    dq_total_tests++;
    if(expected == 1)
        return 1;

    return 0;
}


void add(struct Deque* deque, int value)
{
    int *t = (int*)malloc(sizeof(int));
    *t = value;
    dq_enqueue(deque, t);
}

void dq_test_generate_data(struct Deque* dq)
{
    add(dq, 1);
    add(dq, 2);
    add(dq, 3);
}

void dq_test_release_data(struct Deque* dq)
{
    while(dq_get_total(dq) > 0)
    {
        int* temp = dq_dequeue(dq);
        free(temp);
    }
}

int dq_test_enqueue()
{
    struct Deque dq; //put this away, each test should work without dependency
    dq_init(&dq, 10);

    dq_test_generate_data(&dq);
    int test_data[] = {1,2,3};
    int test_data_size = 3;
    int outcome = dq_test_buffer_equality(&dq, test_data, test_data_size);

    dq_test_release_data(&dq);
    dq_release(&dq);
    return outcome;
}

int dq_test_dequeue()
{
    struct Deque dq; //put this away, each test should work without dependency
    dq_init(&dq, 10);

    int outcome = 0;
    dq_test_generate_data(&dq);
    int test_data[] = {2,3};
    int test_data_size = 2;
    
    int* result = dq_dequeue(&dq);
    outcome += dq_test_buffer_equality(&dq, test_data, test_data_size);
    outcome += dq_test_value_equality(*result, 1);


    free(result);
    dq_test_release_data(&dq);
    dq_release(&dq);

    return outcome;
}

int dq_test_pop_tail()
{
    struct Deque dq;
    dq_init(&dq, 10);

    int outcome = 0;
    dq_test_generate_data(&dq);
    int test_data[] = {1,2};
    int test_data_size = 2;
    
    int* result = dq_pop_tail(&dq);
    outcome += dq_test_buffer_equality(&dq, test_data, test_data_size);
    outcome += dq_test_value_equality(*result, 1);

    free(result);
    dq_test_release_data(&dq);
    dq_release(&dq);

    return outcome;
}

int dq_test_dequeue_when_deque_empty()
{
    dq_total_tests++;
    struct Deque dq;
    dq_init(&dq, 10);

    if(dq_dequeue(&dq) == 0)
       return 1; 
    //VERIFY deque fields?
    dq_release(&dq);
    return 0;
}


int dq_test_pop_tail_when_deque_empty()
{
    dq_total_tests++;
    struct Deque dq;
    dq_init(&dq, 10);

    if(dq_pop_tail(&dq) == 0)
       return 1; 
    //VERIFY deque fields?
    dq_release(&dq);
    return 0;
}

int dq_test_enqueue_when_deque_full()
{
    struct Deque dq; 
    dq_init(&dq, 2);
    add(&dq, 1);
    add(&dq, 2);
    add(&dq, 1);

    int test_data[] = {1,2};
    int test_data_size = 2;
    int outcome = dq_test_buffer_equality(&dq, test_data, test_data_size);

    dq_test_release_data(&dq);
    dq_release(&dq);
    
    return outcome;
}

int main()
{
    int outcome = 0;

    outcome += dq_test_enqueue();
    outcome += dq_test_dequeue();   
    outcome += dq_test_pop_tail();

    outcome += dq_test_dequeue_when_deque_empty();
    outcome += dq_test_pop_tail_when_deque_empty();
    outcome += dq_test_enqueue_when_deque_full(); //Possibly need some verbosity levels here

    printf("\nTest Passed: %d / %d\n", outcome, dq_total_tests);
    printf("Test Failed: %d\n", dq_total_tests - outcome);

    return 0;
}

