#include "cursor.h"

void cursor_default(struct Cursor* cursor)
{
    cursor->x = 0;
    cursor->y = 0;
    cursor->index = 0;
    cursor->line = 0;
}

void cursor_init(struct Cursor* cursor, int x, int y, int index, int line)
{
//    cursor_default();
    *cursor = (struct Cursor){ .x = x, .y = y, .index = index, .line = line };
}
