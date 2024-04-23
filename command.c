#include "command.h"

void default_command(struct Command* cmd)
{
    cmd->child = 0;
    cmd->execute = 0;
}

void init_command(struct Command* command, void* child, void (*execute)(struct Command*))
{
    default_command(command);
    command->child = child;
    command->execute = execute;
}
