#ifndef HISTORY_H
#define HISTORY_H

#define HISTORY_SIZE 10

#include "deque.h"
#include "command.h"

struct History
{
    struct Deque deque;
};

void history_init(struct History* history);
void history_release(struct History* history);
void history_add(struct History* history, struct Command* command);
void history_undo(struct History* history);


#endif

