#pragma once
#include "../lib/hw.h"

class ThreadControlBlock;
#include "ThreadControlBlock.hpp"
typedef ThreadControlBlock* thread_t;

class Sem;
#include "Sem.h"
typedef Sem* sem_t;


enum Syscall{
    MEM_ALLOC = 0x01,
    MEM_FREE = 0x02,
    THREAD_CREATE = 0x11,
    THREAD_EXIT = 0x12,
    THREAD_DISPATCH = 0x13,
    SEM_OPEN = 0x21,
    SEM_CLOSE = 0x22,
    SEM_WAIT = 0x23,
    SEM_SIGNAL = 0x24,
    SEM_TIMED_WAIT = 0x25,
    SEM_TRY_WAIT = 0x26,
    TIME_SLEEP = 0x31,
    GET_CHAR = 0x41,
    PUT_CHAR = 0x42
};

void syscall_handler(size_t syscall_code, size_t arg1, size_t arg2, size_t arg3);

void* mem_alloc(size_t size);

int mem_free(void* mem_block);

int thread_create(thread_t* handle, void (*start_routine)(void*), void* arg);

int thread_exit();

void thread_dispatch();

int sem_open(sem_t* handle, unsigned init);

int sem_close(sem_t handle);

int sem_wait(sem_t id);

int sem_signal(sem_t id);

int sem_timedwait(sem_t id, time_t timeout);

int sem_trywait(sem_t id);

int time_sleep(time_t);

const int EOF = -1;

char getc();

void putc(char chr);