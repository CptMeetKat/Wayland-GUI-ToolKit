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


void execute_insert(struct Command* c);
void release_command_insert(struct Command* c);
struct Command_Insert* cmd_insert_create(struct TextField* textfield,
                         char new_char,
                         int position);
#endif
