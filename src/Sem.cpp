//
// Created by os on 5/5/24.
//

#include "../h/Sem.h"
#include "../h/ThreadControlBlock.hpp"
#include "../h/Scheduler.h"
#include "../h/MemoryAllocator.h"

LinkedList<Sem> *Sem::open_semaphores = nullptr;

int Sem::sem_open(Sem **sem, size_t init) {
    *sem = new Sem(init);
    if(*sem == nullptr) return -1; // Could not create semaphore
    open_semaphores->push_back(*sem);
    return 0;
}

int Sem::sem_close(Sem *sem) {
    if(sem == nullptr) return -1;
    sem->open = false;
    while(sem->val<0){
        sem_signal(sem);
    }
    open_semaphores->delete_element(sem);
    delete sem->waiting;
    delete sem->sleeping;
    delete sem;
    return 0;
}

int Sem::sem_wait(Sem *sem) {
    if(sem == nullptr || !sem->is_open()) return -1;
    if(--sem->val < 0){
        sem->block();
    }
    return 0;
}

int Sem::sem_signal(Sem *sem) {
    if(sem == nullptr) return -1;
    if(sem->val < 0){
        if(sem->waiting->front()) sem->deblock();
        else if(!sem->sleeping->is_empty()) sem->sleeping->awake_first((sem->is_open()? ThreadControlBlock::NONE : ThreadControlBlock::SEMDEAD));
    }
    sem->val++;
    return (!sem->is_open()?-1:0);
}

int Sem::sem_timedwait(Sem *sem, time_t time) {
    if(sem == nullptr) return -3;
    if(time == 0) return 0;
    if(--sem->val < 0) {
        ThreadControlBlock::running->set_timeout_cause(ThreadControlBlock::TIMEOUT);
        sem->sleeping->insert(time);
        ThreadControlBlock::yield();
    }
    if(ThreadControlBlock::running->get_timeout_cause() == ThreadControlBlock::SEMDEAD) return SEMDEAD;
    if(ThreadControlBlock::running->get_timeout_cause() == ThreadControlBlock::TIMEOUT) {
        sem->val++;
        return TIMEOUT;
    }
    return 0;
}

int Sem::sem_trywait(Sem *sem) {
    if(sem == nullptr) return -2; // Semaphore does not exist
    if(!sem->is_open()) return -1; // Semaphore closed: can't wait
    if(sem->val <= 0) return 0; // Semaphore open: can't pass;
    return 1; // Semaphore open: can pass
}

void Sem::block() {
    waiting->push_back(ThreadControlBlock::running);
    ThreadControlBlock::running->set_status(ThreadControlBlock::SUSPENDED);
    thread_dispatch();
}

void Sem::deblock() {
    ThreadControlBlock* t = waiting->front();
    if(t!=nullptr) {
        waiting->pop_front();
        t->set_status(ThreadControlBlock::RUNNING);
        Scheduler::put(t);
    }
}

Sem::Sem(size_t init)
    :val(init), open(true){
    waiting = new SuspendedThreadQueue();
    sleeping = new SleepingThreadQueue();
    if(!open_semaphores) open_semaphores = new LinkedList<Sem>();
}

bool Sem::is_open() {
    return open;
}

void *Sem::operator new(size_t size) {
    return MemoryAllocator::kernel_malloc(size);
}

void Sem::operator delete(void *mem_block) {
    MemoryAllocator::kernel_free(mem_block);
}

void *Sem::operator new[](size_t size) {
    return MemoryAllocator::kernel_malloc(size);
}

void Sem::operator delete[](void *mem_block) {
    MemoryAllocator::kernel_free(mem_block);
}

void Sem::decrement_timers() {
    LinkedList<Sem>::Node* curr = open_semaphores->head;
    while(curr){
        curr->val->sleeping->decrement_time();
        curr->val->sleeping->remove_non_sleeping();
        curr = curr->next;
    }
}


