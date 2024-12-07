#pragma once
#include "syscall_c.hpp"
#include "LinkedList.h"
#include "ThreadControlBlock.hpp"
#include "ThreadSleep.hpp"

typedef LinkedList<ThreadControlBlock> SuspendedThreadQueue;
typedef ThreadSleep SleepingThreadQueue;

class Sem {
public:
    static LinkedList<Sem> *open_semaphores;

    static int sem_open(Sem** sem, size_t init);
    static int sem_close(Sem* sem);

    static int sem_wait(Sem* sem);
    static int sem_signal(Sem* sem);

    static int sem_timedwait(Sem* sem, time_t time);
    static int sem_trywait(Sem* sem);

    static void decrement_timers();
    bool is_open();

    void* operator new(size_t size);
    void operator delete(void *mem_block);
    void* operator new[](size_t size);
    void operator delete[](void* mem_block);
protected:
    void deblock();
    void block();

private:
    enum Signals{
        SEMDEAD = -1,
        TIMEOUT = -2
    };

    explicit Sem(size_t init);
    virtual ~Sem() = default;

    SuspendedThreadQueue *waiting;
    SleepingThreadQueue *sleeping;

    int val;
    bool open;
};