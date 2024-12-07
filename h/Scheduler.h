#pragma once

#include "ThreadControlBlock.hpp"
#include "LinkedList.h"

typedef LinkedList<ThreadControlBlock> ThreadQueue;

class Scheduler {
public:
    static ThreadControlBlock* get();
    static void put(ThreadControlBlock *t);

    static ThreadControlBlock* first();

    void* operator new(size_t size);
    void operator delete(void *mem_block);
private:
    static ThreadQueue *running_threads;
};

