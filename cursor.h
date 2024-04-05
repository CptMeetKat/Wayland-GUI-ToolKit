

#ifndef GUI_CURSOR_H 
#define GUI_CURSOR_H 

struct Cursor
{
    int x;
    int y;
    int index;
    int line;
};


void cursor_init(struct Cursor* cursor, int x, int y, int index, int line);

#endif 
