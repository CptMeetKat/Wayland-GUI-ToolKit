#include <stdio.h>
#include "timed-event.h"

void check(struct TimedEventContainer* t, int time)
{
    for(int i = 0; i < t->total_events; i++)
    {
        struct Event* event = &(t->events[i]);

        if(event->time < time)
        {
            printf("%d %d %d\n", event->time, time, event->interval);
            event->run(event->object, event->args);
            event->time = time + event->interval; 
        }
    }
}

void createEvent(struct TimedEventContainer* container, struct Event e)
{
    if(container->total_events < 32) //magic number 32 represents the max capacity of static events array
    {
        container->events[container->total_events] = e;
        container->total_events += 1;
    }
}
