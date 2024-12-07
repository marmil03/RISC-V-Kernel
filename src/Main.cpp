#include "../h/Console.h"
#include "../h/MemoryAllocator.h"
#include "../h/Scheduler.h"
#include "../h/RiscV.h"
#include "../h/syscall_c.hpp"
#include "../h/syscall_cpp.hpp"
#include "../h/Sem.h"

[[noreturn]]
void busy_wait(void*){
    while(true){
        if(Scheduler::first()){
            thread_dispatch();
        }
    }
}

void userMain();

void user_wrapper(void *){
    userMain();
}

int main(){
    MemoryAllocator::create_memory_allocator();
    IO::create_io_system();
    RiscV::write_stvec((size_t) &RiscV::supervisor_trap);

    ThreadControlBlock *busy_wait_thread, *printer_thread;
    thread_create(&ThreadControlBlock::running, nullptr, nullptr);
    thread_create(&busy_wait_thread, &busy_wait, nullptr);
    thread_create(&printer_thread, &printer_wrapper, nullptr);

    RiscV::mask_set_sstatus(RiscV::SSTATUS_SIE);
    RiscV::set_user_mode(true);

    ThreadControlBlock *user;
    thread_create(&user, user_wrapper, nullptr);

    while(user->get_status() != ThreadControlBlock::COMPLETE){
        thread_dispatch();
    }

    RiscV::set_user_mode(false);
    puts("Test prosao\n");
    while(!IO::output.empty()) {}
    return 0;
}