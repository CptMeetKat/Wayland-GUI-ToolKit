//#include <ft2build.h>
//#include FT_FREETYPE_H

#include "xdg-shell-client-protocol.h" //these are only included for an enum, mb unnecessary coupling
#include "gui-textfield.h"
#include "gui-widget.h"



void draw(struct Widget* widget, uint32_t *data, int w_width, int w_height)
{
    if(widget->type == TEXTBOX)
    {
        struct TextField* t =  widget->child; 
        t->draw(widget, data, w_width, w_height);
    }
    //else if ( OTHER COMPONENT )
}


void key_press(struct Widget* widget, uint32_t state, int sym)
{
    if(widget->type == TEXTBOX)
    {
        struct TextField* t = widget->child;
        t->key_press(t,state,sym);
    }
}

void focus_widget(struct Widget* widget)
{
    widget->isFocused = 1;
    if(widget->type == TEXTBOX)
    {
        struct TextField* t = widget->child;
        t->focus(t);
    }
}
