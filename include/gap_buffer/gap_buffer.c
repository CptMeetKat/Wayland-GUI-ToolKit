#include <stdio.h>
#include <stdlib.h>
#include "gap_buffer.h"

static void flip_left_to_right(struct GapBuffer* gb);
static void flip_right_to_left(struct GapBuffer* gb);

void gb_print_left(struct GapBuffer* gb)
{
    printf("L: ");
    for(int i = 0; i < gb->next_left; i++)
    {
        printf("%c", gb->buffer_left[i]);
    }
    printf("\n");
}

void gb_print_right(struct GapBuffer *gb)
{
    printf("R: ");
    for(int i = gb->next_right+1; i < gb->capacity - 1; i++)
    {
        printf("%c", gb->buffer_right[i]);
    }
    printf("\n");
}

void gb_gap_buffer_default(struct GapBuffer* gb)
{
    gb->buffer_left = NULL; //There are no NULL safeties in any of this code
    gb->buffer_right = NULL;

    gb->capacity = 0;
    gb->next_left = 0;
    gb->next_right = 0;
    gb->size = 0;   
}

void gb_gap_buffer_init(struct GapBuffer* gb, int capacity)
{
    gb_gap_buffer_default(gb);
    gb->buffer_left = (char*)malloc(sizeof(char) * capacity); //Add memalloc safety
    gb->buffer_right = (char*)malloc(sizeof(char) * capacity); //Add memalloc safety
    gb->capacity = capacity;
    
    gb->next_left = 0;
    gb->buffer_left[0] = '\0';

    gb->next_right = gb->capacity - 2;
    gb->buffer_right[gb->capacity - 1] = '\0';
    gb->size = 0;   
}

void gb_release(struct GapBuffer* gb)
{
    free(gb->buffer_left);
    free(gb->buffer_right);
    gb->buffer_left = NULL;
    gb->buffer_right = NULL;
    gb->capacity = 0;
    gb->size = 0;
}

int gb_insert(struct GapBuffer* gb, char new_char, int position)
{
    if(gb->size >= gb->capacity - 1)
    {
        printf("Warning: GapBuffer max storage reached\n");
        return 0;
    }

    if(position < 0 || position > gb->size)
    {
        printf("Warning: GapBuffer inserting data out of range\n");
        return 0;
    }


    if(gb->next_left > position)
    {
        while(gb->next_left != position)
            flip_left_to_right(gb);
    }
    else
    {
        while(gb->next_left != position)
            flip_right_to_left(gb);
    }
    gb->buffer_left[gb->next_left++] = new_char;
    gb->buffer_left[gb->next_left] = '\0';
    gb->size++;

    return 1;
}

char gb_get(struct GapBuffer* gb, int index)
{
    //How to make safer?
    if(index < gb->next_left)   
        return gb->buffer_left[index];
    else
        return gb->buffer_right[ gb->next_right + 1 + (index - gb->next_left ) ];
}


static void flip_left_to_right(struct GapBuffer* gb)
{
    if(gb->size <= 0)
        return;

     gb->buffer_right[gb->next_right] = gb->buffer_left[gb->next_left - 1];
     gb->buffer_left[gb->next_left-1] = '\0';
     gb->next_left--;
     gb->next_right--;
}


static void flip_right_to_left(struct GapBuffer* gb)
{
if(gb->size <= 0)
        return;

     gb->buffer_left[gb->next_left] = gb->buffer_right[gb->next_right + 1];
     gb->buffer_left[gb->next_left+1] = '\0';
     gb->next_left++;  
     gb->next_right++; 
}

void gb_print(struct GapBuffer* gb)
{
    for(int i = 0; i < gb->size; i++)
    {
        char c = gb_get(gb,i);
        printf("%c", c);
    }
    printf("\n");
}



void gb_append(struct GapBuffer *gb, char new_char)
{
    gb_insert(gb, new_char, gb->size);
}


void gb_set_text(struct GapBuffer* gb, char* text, int text_length)
{
    for(int i = 0; i < text_length; i++)
    {
        gb_append(gb, text[i]);
    }
}

int gb_remove(struct GapBuffer *gb, int position, char* out)
{
    if(gb->size <= 0)
    {
        printf("Warning: Deleteing from empty buffer\n");
        return 0;
    }

    if(position < 0 || position >= gb->size)
    {
        printf("Warning: Trying to delete out of range\n");
        return 0;
    }



    if(gb->next_left-1 > position)
    {
        while(gb->next_left -1 != position)
            flip_left_to_right(gb);
    }
    else
    {
        while(gb->next_left -1 != position)
            flip_right_to_left(gb);
    }

    *out = gb->buffer_left[gb->next_left-1];
    gb->buffer_left[gb->next_left-1] = '\0';
    gb->next_left--;
    gb->size--;
    return 1;
}


