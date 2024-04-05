


#ifndef GUI_WIDGET_H
#define GUI_WIDGET_H  

#include <stdint.h>

enum ComponentType {
    TEXTBOX, //Need to modify name
    SQUARE,
    NOTYPE
}; //This enum may not be needed anymore

struct Widget {
    int x;
    int y; 
    int height;
    int width;
    int order; 
    int isFocused;

    enum ComponentType type;
    void *child;
    void (*draw)(struct Widget*, uint32_t*, int, int);
    void (*key_press)(struct Widget*, uint32_t state, int);
    void (*focus)(struct Widget*);
};


void key_press(struct Widget*, uint32_t state, int);
void draw(struct Widget* widget, uint32_t *data, int w_width, int w_height);
void focus_widget(struct Widget* widget);

void init_default_widget(struct Widget* widget);
void init_widget(struct Widget* widget,
                 int x,
                 int y,
                 int height,
                 int width,
                 enum ComponentType type,
                 void* child,
                 void (*draw)(struct Widget*, uint32_t*, int, int),
                 void (*key_press)(struct Widget*, uint32_t state, int),
                 void (*focus)(struct Widget*)
                 );

#endif 
