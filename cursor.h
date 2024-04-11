

#ifndef GUI_CURSOR_H 
#define GUI_CURSOR_H 

struct Cursor
{
    int x;
    int y;
    int index;
    int line;
    int cursor_visible;
};


void cursor_init(struct Cursor* cursor, int x, int y, int index, int line);
void cursor_default(struct Cursor* cursor);
void toggle_cursor(struct Cursor* c);
#endif 
