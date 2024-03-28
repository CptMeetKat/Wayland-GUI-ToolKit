


#ifndef GAP_BUFFER_H
#define GAP_BUFFER_H 



struct GapBuffer
{
    char* buffer_left; //Need to make this dynamic
    char* buffer_right;
    int next_left;
    int next_right;
    int size;
    int capacity;
};


int gb_insert(struct GapBuffer* gb, char new_char, int position);
void gb_append(struct GapBuffer *gb, char new_char);
int gb_remove(struct GapBuffer *gb, int position);
void gb_gap_buffer_init(struct GapBuffer* gb, int capacity);
void gb_gap_buffer_default(struct GapBuffer* gb);
char gb_get(struct GapBuffer* gb, int index);
void gb_set_text(struct GapBuffer* gb, char* text, int text_length);
void gb_release(struct GapBuffer* gb);
void gb_print(struct GapBuffer* gb);

#endif
