#include "command_remove.h"
#include "gui-textfield.h"

void cmd_remove_release(struct Command* c)
{
    struct Command_Remove* ci = c->child;
    free(ci->base); //Unsure of parent then child or child then parent order.....
    free(ci);
}

struct Command_Remove* cmd_remove_create(struct TextField* textfield,
                         char removed_char,
                         int position)
{
    struct Command_Remove* cmd_remove = 
        (struct Command_Remove*)malloc(sizeof(struct Command_Remove*));
    cmd_remove->removed_char = removed_char;
    cmd_remove->position = position;
    cmd_remove->textfield = textfield;
    cmd_remove->base = (struct Command*)malloc(sizeof(struct Command*));
    cmd_remove->base->child = cmd_remove;
    cmd_remove->base->execute = cmd_remove_execute;
    cmd_remove->base->release = cmd_remove_release;
        
    return cmd_remove;
}

void cmd_remove_execute(struct Command* c)
{
    struct Command_Remove* ci = c->child;
    remove_char(ci->textfield, ci->position, 0);
    set_cursor_position(ci->textfield, ci->position);

//    insert_char(ci->textfield, ci->new_char, ci->position);
}



