


#ifndef GAP_BUFFER_H
#define GAP_BUFFER_H 

#define BUFFER_SIZE 128 //Need to make this dynamic


struct GapBuffer
{
    char buffer_left[BUFFER_SIZE]; //Need to make this dynamic
    char buffer_right[BUFFER_SIZE];
    int next_left;
    int next_right;
    int size;
};


void gb_insert(struct GapBuffer* gb, char new_char, int position);
void gb_append(struct GapBuffer *gb, char new_char);
void gb_remove(struct GapBuffer *gb, int position);
void gb_gap_buffer_init(struct GapBuffer* gb);
char gb_get(struct GapBuffer* gb, int index);

void gb_print(struct GapBuffer* gb);


#endif
