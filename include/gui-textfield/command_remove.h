#ifndef COMMAND_REMOVE_H
#define COMMAND_REMOVE_H

#include "command.h"
#include "gui-textfield.h"

struct Command_Remove
{
    struct Command* base;
    struct TextField* textfield;
    char removed_char;
    int position;
};


void cmd_remove_execute(struct Command* c);
void cmd_remove_release(struct Command* c);
struct Command_Remove* cmd_remove_create(struct TextField* textfield,
                         char removed_char,
                         int position);
struct Command* cmd_remove_invert(struct Command* command);
#endif
