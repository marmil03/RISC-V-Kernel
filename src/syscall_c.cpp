#include "../h/syscall_c.hpp"

void syscall_handler(size_t syscall_code, size_t arg1 = 0, size_t arg2 = 0, size_t arg3 = 0, size_t arg4 = 0){
    __asm__ volatile("ecall");
}

void* mem_alloc(size_t size){
    if(size<=0) return nullptr;
    void* volatile mem_block;
    syscall_handler(MEM_ALLOC, size);
    __asm__ volatile("mv %[mem_block], a0": [mem_block] "=r"(mem_block));
    return mem_block;
}

int mem_free(void* mem_block){
    if(mem_block == nullptr) return 0;
    int volatile status;
    syscall_handler(MEM_FREE, (size_t) mem_block);
    __asm__ volatile("mv %[status], a0": [status] "=r"(status));
    return status;
}

int thread_create(thread_t* handle, void (*start_routine)(void*), void* arg){
    void* stack_space = (start_routine == nullptr? nullptr: (mem_alloc(DEFAULT_STACK_SIZE)));
    int volatile status;
    syscall_handler(THREAD_CREATE, (size_t) handle, (size_t) start_routine, (size_t) arg, (size_t) stack_space);
    __asm__ volatile("mv %[status], a0": [status] "=r"(status));
    return status;
}


void thread_dispatch(){
    syscall_handler(THREAD_DISPATCH);
}

int thread_exit(){
    int volatile status;
    syscall_handler(THREAD_EXIT);
    __asm__ volatile("mv %[status], a0": [status] "=r"(status));
    return status;
}

int sem_open(sem_t* handle, unsigned init){
    int volatile status;
    syscall_handler(SEM_OPEN, (size_t)handle, init);
    __asm__ volatile("mv %[status], a0": [status] "=r"(status));
    return status;
}

int sem_close(sem_t handle){
    int volatile status;
    syscall_handler(SEM_CLOSE, (size_t)handle);
    __asm__ volatile("mv %[status], a0": [status] "=r"(status));
    return status;
}

int sem_wait(sem_t id){
    int volatile status;
    syscall_handler(SEM_WAIT, (size_t)id);
    __asm__ volatile("mv %[status], a0": [status] "=r"(status));
    return status;
}

int sem_signal(sem_t id){
    int volatile status;
    syscall_handler(SEM_SIGNAL, (size_t)id);
    __asm__ volatile("mv %[status], a0": [status] "=r"(status));
    return status;
}

int sem_timedwait(sem_t id, size_t time){
    int volatile status;
    syscall_handler(SEM_TIMED_WAIT, (size_t)id, (size_t)time);
    __asm__ volatile("mv %[status], a0": [status] "=r"(status));
    return status;
}

int sem_trywait(sem_t id){
    int volatile status;
    syscall_handler(SEM_TRY_WAIT, (size_t)id);
    __asm__ volatile("mv %[status], a0": [status] "=r"(status));
    return status;
}

int time_sleep(time_t time){
    int volatile status;
    syscall_handler(TIME_SLEEP, time);
    __asm__ volatile("mv %[status], a0": [status] "=r"(status));
    return status;
}

char getc(){
    char volatile chr;
    syscall_handler(GET_CHAR);
    __asm__ volatile("mv %[chr], a0": [chr] "=r"(chr));
    return chr;
}

void putc(char chr){
    syscall_handler(PUT_CHAR, chr);
}