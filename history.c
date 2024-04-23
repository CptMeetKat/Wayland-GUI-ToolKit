#include "history.h"

void history_init(struct History* history)
{
   dq_init(&(history->deque), HISTORY_SIZE); 
}

void history_release(struct History* history)
{
    while( dq_get_total(&history->deque) > 0 )
    {
        struct Command* last_action = dq_pop_tail(&(history->deque));
        last_action->release(last_action);
    }
    dq_release(&(history->deque));
}

void history_add(struct History* history, struct Command* command)
{
    while( dq_get_total(&history->deque) >= history->deque.size ) //only retain latest x history
    {
       struct Command* old_command = dq_dequeue(&(history->deque));
       old_command->release(old_command);
    }
    dq_enqueue(&(history->deque), command);
}

void history_undo(struct History* history)
{
    struct Command* last_action = dq_pop_tail(&(history->deque));
    last_action->execute(last_action);
    last_action->release(last_action);
}


