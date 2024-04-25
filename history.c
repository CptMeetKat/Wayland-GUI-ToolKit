#include "history.h"

void history_init(struct History* history)
{
   dq_init(&(history->deque), HISTORY_SIZE); 
   dq_init(&(history->redo_commands), HISTORY_SIZE); 
}

void history_empty_dq(struct Deque* dq)
{
    while( dq_get_total(dq) > 0 )
    {
        struct Command* last_action = dq_pop_tail(dq);
        last_action->release(last_action);
    }
}

void history_release_dq(struct Deque* dq)
{
    history_empty_dq(dq);
    dq_release(dq);
}


void history_release(struct History* history)
{
    history_release_dq(&(history->deque));
    history_release_dq(&(history->redo_commands));
}


void history_add(struct History* history, struct Command* command)
{
    history_empty_dq(&(history->redo_commands));

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
    if(last_action == 0)
        return;
    last_action->execute(last_action);

    dq_enqueue(&(history->redo_commands), last_action->invert(last_action));
    last_action->release(last_action); 
}

void history_redo(struct History* history)
{
    struct Command* repeat_action = dq_pop_tail(&(history->redo_commands));
    if(repeat_action == 0)
        return;


    //invert command
    repeat_action->execute(repeat_action);
    struct Command* inverted_action = repeat_action->invert(repeat_action);
    dq_enqueue(&(history->deque), inverted_action);

    repeat_action->release(repeat_action);
}








