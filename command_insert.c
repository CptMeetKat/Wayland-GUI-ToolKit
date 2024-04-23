#include "command_insert.h"

void release_command_insert(struct Command* c)
{
    struct Command_Insert* ci = c->child;
    free(ci->base); //Unsure of parent then child or child then parent order.....
    free(ci);
}

struct Command_Insert* init_cmd_insert(struct TextField* textfield,
                         char new_char,
                         int position)
{
    struct Command_Insert* cmd_insert = 
        (struct Command_Insert*)malloc(sizeof(struct Command_Insert*));
    cmd_insert->new_char = new_char;
    cmd_insert->position = position;

    cmd_insert->base = (struct Command*)malloc(sizeof(struct Command*));
    cmd_insert->base->child = cmd_insert;
    cmd_insert->base->execute = execute_insert;
    cmd_insert->base->release = release_command_insert;
        
    return cmd_insert;
}

void execute_insert(struct Command* c)
{
    struct Command_Insert * ci = c->child;
    insert_char(ci->textfield, ci->new_char, ci->position);
}



