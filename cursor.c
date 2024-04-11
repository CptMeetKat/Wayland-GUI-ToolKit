#include "cursor.h"

void cursor_default(struct Cursor* cursor)
{
    cursor->x = 0;
    cursor->y = 0;
    cursor->index = 0;
    cursor->line = 0;
    cursor->cursor_visible = 0;
}

void cursor_init(struct Cursor* cursor, int x, int y, int index, int line)
{
//    cursor_default();
    *cursor = (struct Cursor){ .x = x, .y = y, .index = index, .line = line, .cursor_visible = 0 };
}


void toggle_cursor(struct Cursor* c)
{
    if(c->cursor_visible)
        c->cursor_visible = 0;
    else
        c->cursor_visible = 1;
}
