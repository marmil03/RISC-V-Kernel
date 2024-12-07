//
// Created by os on 5/5/24.
//

#include "../h/Scheduler.h"
#include "../h/MemoryAllocator.h"

ThreadQueue *Scheduler::running_threads = nullptr;

ThreadControlBlock *Scheduler::get() {
    ThreadControlBlock* t = running_threads->front();
    running_threads->pop_front();
    return t;
}

void Scheduler::put(ThreadControlBlock *t) {
    if(!running_threads) running_threads = new ThreadQueue();
    running_threads->push_back(t);
}

ThreadControlBlock *Scheduler::first() {
    return running_threads->front();
}

void *Scheduler::operator new(size_t size) {
    return MemoryAllocator::kernel_malloc(size);
}

void Scheduler::operator delete(void *mem_block) {
    MemoryAllocator::kernel_free(mem_block);
}
