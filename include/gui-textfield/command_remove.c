#include "command_remove.h"
#include "gui-textfield.h"
#include "command_insert.h"

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
        (struct Command_Remove*)malloc(sizeof(struct Command_Remove));
    cmd_remove->removed_char = removed_char;
    cmd_remove->position = position;
    cmd_remove->textfield = textfield;
    cmd_remove->base = (struct Command*)malloc(sizeof(struct Command));
    cmd_remove->base->child = cmd_remove;
    cmd_remove->base->execute = cmd_remove_execute;
    cmd_remove->base->release = cmd_remove_release;
    cmd_remove->base->invert = cmd_remove_invert; 
    return cmd_remove;
}

void cmd_remove_execute(struct Command* c)
{
    struct Command_Remove* ci = c->child;
    remove_char(ci->textfield, ci->position, 0);
    set_cursor_position(ci->textfield, ci->position);
}



struct Command* cmd_remove_invert(struct Command* command)
{
    if(command == 0)
        return 0;

    struct Command_Remove* cr = command->child;
    struct Command_Insert* ci = cmd_insert_create(cr->textfield, cr->removed_char, cr->position);
    return ci->base;
}
