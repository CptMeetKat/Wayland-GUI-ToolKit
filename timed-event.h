
#ifndef TIME_EVENT_H
#define TIME_EVENT_H

struct Event
{
    int time;
    int interval;
    void* object;
    void* args;
    void (*run)(void*, void*);
};

struct TimedEventContainer
{
    struct Event events[32]; //not dynamic
    int total_events;
};


void check(struct TimedEventContainer* t, int time);
void createEvent(struct TimedEventContainer* container, struct Event e);

#endif 
