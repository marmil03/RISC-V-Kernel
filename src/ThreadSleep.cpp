//
// Created by os on 5/12/24.
//

#include "../h/ThreadSleep.hpp"
#include "../h/Scheduler.h"
#include "../h/MemoryAllocator.h"

ThreadSleep::ThreadSleep()
    : head(nullptr){}

void ThreadSleep::insert(time_t time) {
    ThreadControlBlock::running->set_status(ThreadControlBlock::SLEEPING);
    Node* new_node = new Node(ThreadControlBlock::running, time);
    if(!head) {
        head = new_node;
        return;
    }
    time_t total_time = head->time_offset;
    Node* curr = head, *prev = nullptr;
    while(curr && total_time <= time){
        prev = curr;
        curr = curr->next;
        if(curr) total_time+=curr->time_offset;
    }
    if(prev == nullptr){
        new_node->next = head;
        head = new_node;
    }
    else{
        new_node->next = prev->next;
        prev->next = new_node;
        new_node->time_offset -= (total_time - (curr?curr->time_offset:0));
    }
    curr = new_node->next;
    if(curr) curr->time_offset -= new_node->time_offset;

}

time_t ThreadSleep::first() {
    if(!head) return -1; // Wraps around
    return head->time_offset;
}

void ThreadSleep::remove_non_sleeping() {
    while(head && head->time_offset == 0){
        if(head->thread){
            head->thread->set_status(ThreadControlBlock::RUNNING);
            Scheduler::put(head->thread);
        }
        Node* node = head;
        head = head->next;
        delete node;
    }
}

void ThreadSleep::decrement_time() {
    if(head){
        head->time_offset--;
    }
}

void *ThreadSleep::operator new(size_t size) {
    return MemoryAllocator::kernel_malloc(size);
}

void ThreadSleep::operator delete(void *mem_block) {
    MemoryAllocator::kernel_free(mem_block);
}

void *ThreadSleep::operator new[](size_t size) {
    return MemoryAllocator::kernel_malloc(size);
}

void ThreadSleep::operator delete[](void *mem_block) {
    MemoryAllocator::kernel_free(mem_block);
}

void ThreadSleep::awaken_all(ThreadControlBlock::TimeoutCause cause) {
    while(head){
        awake_first(cause);
    }
}

void ThreadSleep::awake_first(ThreadControlBlock::TimeoutCause cause) {
    Node* curr = head;
    head = head->next;
    if(curr->thread){
        curr->thread->set_status(ThreadControlBlock::RUNNING);
        if(curr->time_offset != 0) curr->thread->set_timeout_cause(cause);
        Scheduler::put(curr->thread);
        if(head) head->time_offset += curr->time_offset;
    }
    delete curr;
}

bool ThreadSleep::is_empty() {
    return head == nullptr;
}

