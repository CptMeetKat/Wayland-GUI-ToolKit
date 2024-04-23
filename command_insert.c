#include "command_insert.h"

void cmd_insert_release(struct Command* c)
{
    struct Command_Insert* ci = c->child;
    free(ci->base); //Unsure of parent then child or child then parent order.....
    free(ci);
}

struct Command_Insert* cmd_insert_create(struct TextField* textfield,
                         char new_char,
                         int position)
{
    struct Command_Insert* cmd_insert = 
        (struct Command_Insert*)malloc(sizeof(struct Command_Insert*));
    cmd_insert->new_char = new_char;
    cmd_insert->position = position;
    cmd_insert->textfield = textfield;

    cmd_insert->base = (struct Command*)malloc(sizeof(struct Command*));
    cmd_insert->base->child = cmd_insert;
    cmd_insert->base->execute = cmd_insert_execute;
    cmd_insert->base->release = cmd_insert_release;
        
    return cmd_insert;
}

void cmd_insert_execute(struct Command* c)
{
    struct Command_Insert * ci = c->child;
    insert_char(ci->textfield, ci->new_char, ci->position);
}



