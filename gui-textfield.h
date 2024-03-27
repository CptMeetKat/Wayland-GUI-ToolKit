
#ifndef GUI_TEXTFIELD_H
#define GUI_TEXTFIELD_H  
#define GUI_TEXTFIELD_MAX_TEXT 128
#define MAX_FONT 64 
#define BUFFER_SIZE 128 //Need to make this dynamic
#include <stdint.h>
#include "gap_buffer.h"

struct TextField
{
    struct Widget *base;
    char font[MAX_FONT]; 
    int cursor_visible;
    int cursor_x;
    int cursor_y;
    int cursor_index;
    long int last_blink;


    int cursor_line;
    int total_lines;

    struct GapBuffer gb;

    void (*key_press)(struct TextField*, uint32_t state, int);
    void (*draw)(struct Widget* widget, uint32_t *data, int w_width, int w_height);
    void (*focus)(struct TextField* widget);
    
};


void key_press_textfield(struct TextField* textfield, uint32_t state, int sym);
void draw_textfield(struct Widget* widget, uint32_t *data, int w_width, int w_height);
void toggle_cursor(struct TextField* textfield);
void focus_textfield(struct TextField* textfield);
void release_textfield(struct TextField* textfield);

void init_default_textfield(struct TextField* textfield);
void init_textfield(struct TextField* textfield, char* font, char* text, int text_length, int x, int y, int width, int height);
struct TextField* create_textfield(int x, int y, char font[], int width, int height, char text[]);

#endif 
