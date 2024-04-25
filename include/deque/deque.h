
struct Deque
{
    void** buffer; //might be smarter to isolate, by only using Command*
    int size;
    int first;
    int last;
    int total;
};

void dq_init(struct Deque* deque, int size);
void default_c_queue(struct Deque* deque);
int dq_get_total(struct Deque* deque);
void dq_default(struct Deque* deque);
int dq_enqueue(struct Deque* deque, void* value);
void* dq_dequeue(struct Deque* deque);
void* dq_pop_tail(struct Deque* deque);
void dq_release(struct Deque* deque);
