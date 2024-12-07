
#include "../h/ThreadControlBlock.hpp"
#include "../h/RiscV.h"
#include "../h/Scheduler.h"
#include "../h/ThreadSleep.hpp"

ThreadControlBlock* ThreadControlBlock::running = nullptr;
ThreadSleep *ThreadControlBlock::sleeping = nullptr;
size_t ThreadControlBlock::tid = 0;
size_t ThreadControlBlock::timer = 0;

ThreadControlBlock::ThreadControlBlock(void (*func)(void*), void *arg, void *stack_space)
    :t_id(tid++), func(func), arg(arg), stack((size_t*)stack_space), status(RUNNING), timeout_cause(NONE), user_land(RiscV::is_user_mode()), time_slice(DEFAULT_TIME_SLICE),
    context({(size_t)thread_wrapper, (stack_space != nullptr ? (size_t)((char*)stack + DEFAULT_STACK_SIZE) : 0)}) {
    if(func != nullptr) start();
    if(t_id == 0) ThreadControlBlock::sleeping = new ThreadSleep();
}

void ThreadControlBlock::start() {
    Scheduler::put(this);
}

void ThreadControlBlock::yield() {
    __asm__ volatile("ecall");
}

void ThreadControlBlock::dispatch() {
    ThreadControlBlock* old_thr = ThreadControlBlock::running;
    if(old_thr->status != COMPLETE && old_thr->status != SUSPENDED && old_thr->status != SLEEPING) Scheduler::put(old_thr);
    ThreadControlBlock::running = Scheduler::get();

    if(old_thr != ThreadControlBlock::running)
        context_switch(&old_thr->context, &ThreadControlBlock::running->context);
}

int ThreadControlBlock::thread_create(ThreadControlBlock** handle, void (*fun)(void*), void *arg, void* stack_space){
    *handle = new ThreadControlBlock(fun, arg, stack_space);
    if(*handle == nullptr) return -1; // Error: Could not create thread
    return 0; // Success
}

void ThreadControlBlock::set_status(ThreadControlBlock::Status new_status) {
    status = new_status;
}

ThreadControlBlock::Status ThreadControlBlock::get_status() {
    return status;
}

void ThreadControlBlock::kill() {
    ThreadControlBlock::running->set_status(ThreadControlBlock::COMPLETE);
    ThreadControlBlock::yield();
}

ThreadControlBlock::~ThreadControlBlock() {
    delete stack;
    if(func == nullptr) delete sleeping;
}

size_t ThreadControlBlock::get_tid() const {
    return t_id;
}

/*inline bool ThreadControlBlock::is_user_mode() const {
    return user_land;
}*/

void ThreadControlBlock::thread_wrapper() {
    RiscV::pop_spp_spie();
    if(ThreadControlBlock::running->func != nullptr){
        ThreadControlBlock::running->func(ThreadControlBlock::running->arg);
        kill();
    }
}

size_t ThreadControlBlock::get_time_slice() const {
    return time_slice;
}

ThreadControlBlock::TimeoutCause ThreadControlBlock::get_timeout_cause() {
    return timeout_cause;
}

void ThreadControlBlock::set_timeout_cause(ThreadControlBlock::TimeoutCause cause) {
    timeout_cause = cause;
}


void* ThreadControlBlock::operator new(size_t size) {
    return MemoryAllocator::kernel_malloc(size);
};

void ThreadControlBlock::operator delete(void *mem_block) {
    MemoryAllocator::kernel_free(mem_block);
};

void* ThreadControlBlock::operator new[](size_t size) {
    return MemoryAllocator::kernel_malloc(size);
};

void ThreadControlBlock::operator delete[](void* mem_block) {
    MemoryAllocator::kernel_free(mem_block);
}





