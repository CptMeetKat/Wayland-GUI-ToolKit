#include "xdg-shell-client-protocol.h" //these are only included for an enum, mb unnecessary coupling
#include "gui-textfield.h"
#include "gui-widget.h"


void init_default_widget(struct Widget* widget)
{
    widget->x = 0;
    widget->y = 0;
    widget->height = 0;
    widget->width = 0;
    widget->order = 0;
    widget->isFocused = 0;

    widget->child = NULL;
}

void init_widget(struct Widget* widget,
                 int x,
                 int y,
                 int height,
                 int width,
                 void* child,

                 void (*draw)(struct Widget*, uint32_t*, int, int),
                 void (*key_press)(struct Widget*, uint32_t state, int, int),
                 void (*focus)(struct Widget*)
                  )
{
    init_default_widget(widget);
    widget->x = x;
    widget->y = y;
    widget->height = height;
    widget->width = width;
    widget->child = child;
    widget->draw = draw;
    widget->key_press = key_press;
    widget->focus = focus;
}


