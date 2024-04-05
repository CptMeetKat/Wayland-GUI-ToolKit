#include "xdg-shell-client-protocol.h" //these are only included for an enum, mb unnecessary coupling
#include "gui-textfield.h"
#include "gui-widget.h"



//void draw(struct Widget* widget, uint32_t *data, int w_width, int w_height)
//{
//    if(widget->type == TEXTBOX)
//    {
//        struct TextField* t =  widget->child; 
//        widget->draw(widget, data, w_width, w_height);
//    }
//    //else if ( OTHER COMPONENT )
//}
//
//
//void key_press(struct Widget* widget, uint32_t state, int sym)
//{
//    if(widget->type == TEXTBOX)
//    {
//        struct TextField* t = widget->child;
//        t->key_press(t,state,sym);
//    }
//}
//
//void focus_widget(struct Widget* widget)
//{
//    widget->isFocused = 1;
//    if(widget->type == TEXTBOX)
//    {
//        struct TextField* t = widget->child;
//        t->focus(t);
//    }
//}

void init_default_widget(struct Widget* widget)
{
    widget->x = 0;
    widget->y = 0;
    widget->height = 0;
    widget->width = 0;
    widget->order = 0;
    widget->isFocused = 0;

    widget->type =  NOTYPE;
    widget->child = NULL;
//    widget->draw = draw;
//    widget->key_press = key_press;
//    widget->focus = focus_widget;
}

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
                  )
{
    init_default_widget(widget);
    widget->x = x;
    widget->y = y;
    widget->height = height;
    widget->width = width;
    widget->type = type;
    widget->child = child;
    widget->draw = draw;
    widget->key_press = key_press;
    widget->focus = focus;
}


