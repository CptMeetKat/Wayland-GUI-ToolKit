


#ifndef GUI_WIDGET_H
#define GUI_WIDGET_H  


enum ComponentType {
    TEXTBOX,
    SQUARE
};

struct Widget {
    int x;
    int y; 
    int height;
    int width;
    int order; 
    void (*draw)(struct Widget*, uint32_t*, int, int);
    void (*key_press)(struct Widget*, uint32_t state, int);
    enum ComponentType type;
    void *child;
    int isFocused;
};


void key_press(struct Widget*, uint32_t state, int);
void draw(struct Widget* widget, uint32_t *data, int w_width, int w_height);

#endif 
