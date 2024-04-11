

#ifndef GUI_CURSOR_H 
#define GUI_CURSOR_H 

struct Cursor
{
    int x;
    int y;
    int index;
    int line;
    int cursor_visible;

    
    long int last_blink;
};


void cursor_init(struct Cursor* cursor, int x, int y, int index, int line);
void cursor_default(struct Cursor* cursor);
void toggle_cursor(struct Cursor* c);
void cursor_force_show(struct Cursor* cursor);
#endif 
