
#ifndef COMMAND_H
#define COMMAND_H

struct Command
{
    void* child;
    void (*execute)(struct Command* command);
    void (*release)(struct Command* command);
};


void default_command(struct Command* cmd);
void init_command(struct Command* command, void* child, void (*execute)(struct Command*));


#endif 
