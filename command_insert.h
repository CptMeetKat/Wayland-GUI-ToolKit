#ifndef COMMAND_INSERT_H
#define COMMAND_INSERT_H

#include "command.h"
#include "gui-textfield.h"

struct Command_Insert 
{
    struct Command* base;
    struct TextField* textfield;
    char new_char;
    int position;
};


void cmd_insert_execute(struct Command* c);
void cmd_insert_release(struct Command* c);
struct Command_Insert* cmd_insert_create(struct TextField* textfield,
                         char new_char,
                         int position);
#endif
