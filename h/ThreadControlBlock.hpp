#pragma once
#include "../lib/hw.h"
#include "MemoryAllocator.h"
class ThreadSleep;
class ThreadControlBlock {
public:
    enum Status { COMPLETE, SUSPENDED, SLEEPING, RUNNING };
    enum TimeoutCause { SEMDEAD, TIMEOUT, NONE };

    static int thread_create(ThreadControlBlock** handle, void (*fun)(void*), void *arg, void* stack_space);

    void set_status(Status new_status);
    Status get_status();

    void set_timeout_cause(TimeoutCause cause);
    TimeoutCause get_timeout_cause();

    size_t get_time_slice() const;

    size_t get_tid() const;
    inline bool is_user_mode() const{
        return user_land;
    };

    void start();

    static void yield();
    static void dispatch();

    static void kill();

    static ThreadControlBlock *running;

    virtual ~ThreadControlBlock();

    static size_t timer;

    void* operator new(size_t size);
    void operator delete(void *mem_block);
    void* operator new[](size_t size);
    void operator delete[](void* mem_block);

    static ThreadSleep *sleeping;
private:
    ThreadControlBlock(void (*func)(void*), void *arg, void *stack_space);
    static size_t tid;
    size_t t_id;
    void (*func)(void*);
    void *arg;
    size_t* stack;

    Status status;
    TimeoutCause timeout_cause;

    bool user_land;

    size_t time_slice;

    struct Context{
        size_t return_address;
        size_t sp;
    };
    Context context;

    static void thread_wrapper();
    static void context_switch(Context *old_context, Context *new_context);

};
