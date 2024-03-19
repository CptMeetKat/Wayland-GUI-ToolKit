
#ifndef GUI_TEXTFIELD_H
#define GUI_TEXTFIELD_H  

#include <stdint.h>
struct TextField
{
    struct Widget *base;
    char text[128]; //Eventually will cause run time error
    int text_length;

    void (*key_press)(struct TextField*, uint32_t state, int);
    void (*draw)(struct Widget* widget, uint32_t *data, int stride, int w_width, int w_height);
};


void key_press_textfield(struct TextField* textfield, uint32_t state, int sym);
void draw_textfield(struct Widget* widget, uint32_t *data, int stride, int w_width, int w_height);

struct TextField* create_test_textfield(int x, int y);

#endif 
