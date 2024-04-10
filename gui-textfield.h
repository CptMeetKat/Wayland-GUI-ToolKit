
#ifndef GUI_TEXTFIELD_H
#define GUI_TEXTFIELD_H  
#define MAX_FONT 64 
#define LINE_SPACEING 0
#define MAX_WRAPS 256
#include <stdint.h>
#include "gap_buffer.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include "cursor.h"

struct TextField
{
    struct Widget *base;
    char font[MAX_FONT]; 
    
    int cursor_visible;
    
    struct Cursor cursor;

    long int last_blink;
    int font_height;
    int last_line;

    struct GapBuffer gb;

    FT_Library library;
    FT_Face face;

    int wrap_positions[MAX_WRAPS];
    int total_wraps; 
};


void key_press_textfield(struct Widget* widget, uint32_t state, int sym);
void draw_textfield(struct Widget* widget, uint32_t *data, int w_width, int w_height);
void focus_textfield(struct Widget* widget);

void toggle_cursor(struct TextField* textfield);
void release_textfield(struct TextField* textfield);

void init_default_textfield(struct TextField* textfield);
void init_textfield(struct TextField* textfield, char* font, char* text, int text_length, int x, int y, int width, int height, int max_length,
                    void (*draw)(struct Widget*, uint32_t*, int, int),
                    void (*key_press)(struct Widget*, uint32_t state, int),
                    void (*focus)(struct Widget*)
                    );
struct TextField* create_textfield(int x, int y, char font[], int width, int height, char text[], int max_length);

#endif 
