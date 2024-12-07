//
// Created by os on 5/12/24.
//
#pragma once

#include "ThreadControlBlock.hpp"
#include "MemoryAllocator.h"

class ThreadSleep {
public:
    ThreadSleep();

    ThreadSleep(const ThreadSleep &) = delete;
    ThreadSleep(ThreadSleep &&) = delete;

    void insert(time_t time);

    time_t first();

    void decrement_time();
    void remove_non_sleeping();

    void awaken_all(ThreadControlBlock::TimeoutCause cause);
    void awake_first(ThreadControlBlock::TimeoutCause cause);

    bool is_empty();

    ThreadSleep& operator=(const ThreadSleep&) = delete;
    ThreadSleep& operator=(ThreadSleep&&) = delete;

    void* operator new(size_t size);
    void operator delete(void *mem_block);
    void* operator new[](size_t size);
    void operator delete[](void* mem_block);
private:
    struct Node{
        ThreadControlBlock* thread;
        time_t time_offset;
        Node* next;

        Node(ThreadControlBlock* t, time_t time)
            :thread(t), time_offset(time), next(nullptr) {};
        void *operator new(size_t size) {return MemoryAllocator::kernel_malloc(size);}
        void operator delete(void *mem_block) {MemoryAllocator::kernel_free(mem_block);}
    };
    Node *head;
};
