#include "../h/syscall_cpp.hpp"

void* operator new(size_t size){
    return mem_alloc(size);
}

void* operator new[](size_t size){
    return mem_alloc(size);
}

void operator delete(void *mem_block) {
    mem_free(mem_block);
}

void operator delete[](void *mem_block){
    mem_free(mem_block);
}

Thread::Thread(void (*body)(void *), void *arg)
    :myHandle(nullptr), body(body), arg(arg) {}

Thread::~Thread() {
    if(myHandle->get_status() == ThreadControlBlock::COMPLETE){
        delete myHandle;
    }
}

int Thread::start() {
    return thread_create(&myHandle, body, arg);
}

void Thread::dispatch() {
    thread_dispatch();
}

int Thread::sleep(time_t time) {
    return time_sleep(time);
}

Thread::Thread()
    :myHandle(nullptr), arg(this) {
    body = [](void *thread){
        ((Thread*)thread)->run();
    };
}

Semaphore::Semaphore(unsigned int init)
    :myHandle(nullptr){
    sem_open(&myHandle, init);
}

int Semaphore::wait(){
    return sem_wait(myHandle);
}

int Semaphore::signal(){
    return sem_signal(myHandle);
}

int Semaphore::timedWait(time_t time){
    return sem_timedwait(myHandle, time);
}

int Semaphore::tryWait(){
    return sem_trywait(myHandle);
}

Semaphore::~Semaphore() {
    sem_close(myHandle);
}

char Console::getc() {
    return ::getc();
}

void Console::putc(char chr) {
    ::putc(chr);
}

PeriodicThread::PeriodicThread(time_t period)
    :period(period){}

void PeriodicThread::terminate() {
    thread_exit();
}

[[noreturn]]
void PeriodicThread::run() {
    while(true){
        time_sleep(period);
        periodicActivation();
    }
}