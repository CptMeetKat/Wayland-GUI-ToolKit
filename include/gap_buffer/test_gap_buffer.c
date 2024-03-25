
#include "gap_buffer.h"
#include <stdio.h>
#include <string.h>


int gb_total_tests = 0;

int gb_test_verify(struct GapBuffer* result, char* expected)
{
    gb_total_tests++;
    int expected_length = strlen(expected);
    int gap_buffer_length = result->size;

    if( expected_length != gap_buffer_length)
        return 0;

    for(int i = 0; i < expected_length + 1; i++)
    {
       if(gb_get(result, i) != expected[i])
            return 0;
    } 

    return 1;
}

int main()
{
    int outcome = 0;
    struct GapBuffer gb;
    gb_gap_buffer_init(&gb);

    //Insert Start
    gb_insert(&gb, 'A', 0);
    gb_insert(&gb, 'B', 0);
    gb_insert(&gb, 'C', 0);
    outcome += gb_test_verify(&gb, "CBA");

    //Insert End
    gb_append(&gb, 'a');
    gb_append(&gb, 'b');
    gb_append(&gb, 'c');
    outcome += gb_test_verify(&gb, "CBAabc");

    //Insert Middle
    gb_insert(&gb, 'Z', 1);
    gb_insert(&gb, 'X', 1);
    gb_insert(&gb, 'Y', 1);
    outcome += gb_test_verify(&gb, "CYXZBAabc");

    //Remove Start
    gb_remove(&gb, 0);
    outcome += gb_test_verify(&gb, "YXZBAabc");

    //Remove End
    gb_remove(&gb, 7);
    outcome += gb_test_verify(&gb, "YXZBAab");

    //Remove Middle
    gb_remove(&gb, 2);
    outcome += gb_test_verify(&gb, "YXBAab");

    printf("Test Passed: %d\n", outcome);
    printf("Test Failed: %d\n", gb_total_tests - outcome);

    return 0;
}

