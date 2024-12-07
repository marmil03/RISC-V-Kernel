#include "../h/RiscV.h"
#include "../h/syscall_c.hpp"
#include "../h/ThreadControlBlock.hpp"
#include "../h/ThreadSleep.hpp"
#include "../h/Sem.h"
#include "../h/Console.h"
#include "../h/MemoryAllocator.h"

bool RiscV::user_mode = false;

void RiscV::pop_spp_spie(){
    if(ThreadControlBlock::running->is_user_mode()) RiscV::mask_clear_sstatus(SSTATUS_SPP);
    else RiscV::mask_set_sstatus(SSTATUS_SPP);
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}

[[noreturn]]
void RiscV::printer(){
    while (true){
        while(*((char*)CONSOLE_STATUS) & CONSOLE_TX_STATUS_BIT){
            char c = IO::output.get_char();
            *((char*)CONSOLE_TX_DATA) = c;
        }
    }
}

void RiscV::set_user_mode(bool status) {
    RiscV::user_mode = status;
}

bool RiscV::is_user_mode() {
    return RiscV::user_mode;
}

void RiscV::supervisor_trap_handler() {
    size_t volatile scause = read_scause();
    if(scause == ECALL_FROM_SUPERVISOR || scause == ECALL_FROM_USER){
        size_t volatile sepc = read_sepc() + 4;
        size_t volatile sstatus = read_sstatus();

        size_t volatile syscall_code;
        __asm__ volatile("ld %[syscall_code], 80(s0)": [syscall_code] "=r"(syscall_code));
        switch(syscall_code){
            case MEM_ALLOC:{
                size_t volatile size;
                __asm__ volatile("ld %[size], 88(s0)": [size] "=r"(size));
                MemoryAllocator::kernel_malloc(size);
                break;
            }
            case MEM_FREE:{
                void* volatile mem_block;
                __asm__ volatile("ld %[mem_block], 88(s0)": [mem_block] "=r"(mem_block));
                MemoryAllocator::kernel_free(mem_block);
                break;
            }
            case THREAD_CREATE:{
                ThreadControlBlock**  handler;
                void (* func)(void*) ;
                void* arg;
                void* stack_space;
                __asm__ volatile("ld %[handler], 88(s0)": [handler] "=r"(handler));
                __asm__ volatile("ld %[func], 96(s0)": [func] "=r"(func));
                __asm__ volatile("ld %[arg], 104(s0)": [arg] "=r"(arg));
                __asm__ volatile("ld %[stack_space], 112(s0)": [stack_space] "=r"(stack_space));
                ThreadControlBlock::thread_create(handler, func, arg, stack_space);
                break;
            }
            case THREAD_DISPATCH:{
                ThreadControlBlock::timer = 0;
                ThreadControlBlock::dispatch();
                break;
            }
            case THREAD_EXIT:{
                ThreadControlBlock::kill();
                break;
            }
            case SEM_OPEN:{
                sem_t* volatile handler;
                unsigned volatile init;
                __asm__ volatile("ld %[handler], 88(s0)": [handler] "=r"(handler));
                __asm__ volatile("ld %[init], 96(s0)": [init] "=r"(init));
                Sem::sem_open(handler, init);
                break;
            }
            case SEM_CLOSE:{
                sem_t volatile handler;
                __asm__ volatile("ld %[handler], 88(s0)": [handler] "=r"(handler));
                Sem::sem_close(handler);
                break;
            }
            case SEM_SIGNAL:{
                sem_t volatile handler;
                __asm__ volatile("ld %[handler], 88(s0)": [handler] "=r"(handler));
                Sem::sem_signal(handler);
                break;
            }
            case SEM_WAIT:{
                sem_t volatile handler;
                __asm__ volatile("ld %[handler], 88(s0)": [handler] "=r"(handler));
                Sem::sem_wait(handler);
                break;
            }
            case SEM_TIMED_WAIT:{
                sem_t volatile handler;
                time_t time;
                __asm__ volatile("ld %[handler], 88(s0)": [handler] "=r"(handler));
                __asm__ volatile("ld %[time], 96(s0)": [time] "=r"(time));
                Sem::sem_timedwait(handler, time);
                break;
            }
            case SEM_TRY_WAIT:{
                sem_t volatile handler;
                __asm__ volatile("ld %[handler], 88(s0)": [handler] "=r"(handler));
                Sem::sem_trywait(handler);
                break;
            }
            case TIME_SLEEP:{
                time_t volatile time;
                __asm__ volatile("ld %[time], 88(s0)": [time] "=r"(time));
               if(time!=0){
                   ThreadControlBlock::sleeping->insert(time);
                   ThreadControlBlock::timer = 0;
                   ThreadControlBlock::dispatch();
               }
                break;
            }
            case PUT_CHAR:{
                char volatile c;
                __asm__ volatile("ld %[c], 88(s0)": [c] "=r"(c));
                IO::put_char(c);
                break;
            }
            case GET_CHAR:{
                IO::get_char();
                break;
            }
            default:
                ThreadControlBlock::timer = 0;
                ThreadControlBlock::dispatch();

        }
        __asm__ volatile("sd a0, 80(s0)");
        write_sepc(sepc);
        write_sstatus(sstatus);

    }
    else if(scause == SUPERVISOR_SOFTWARE_INTERRUPT){
        Sem::decrement_timers();
        ThreadControlBlock::sleeping->decrement_time();
        ThreadControlBlock::sleeping->remove_non_sleeping();
        ThreadControlBlock::timer++;
        if(ThreadControlBlock::timer >= ThreadControlBlock::running->get_time_slice()) {
            size_t volatile sepc = read_sepc();
            size_t volatile sstatus = read_sstatus();

            ThreadControlBlock::timer = 0;
            ThreadControlBlock::dispatch();

            write_sepc(sepc);
            write_sstatus(sstatus);
        }
        mask_clear_sip(SIP_SSIP);
    }
    else if (scause == SUPERVISOR_EXTERNAL_INTERRUPT){
        if(plic_claim() == CONSOLE_IRQ){
            while(*((char*)CONSOLE_STATUS) & CONSOLE_RX_STATUS_BIT){
                char c = *((char*)CONSOLE_RX_DATA);
                IO::input.put_char(c);
            }
            plic_complete(CONSOLE_IRQ);
        }
    }

    else{
        puts("\nError: Unexpected exception\n");
        puts("Scause:");
        putint(scause);
        putc('\n');
        while(!IO::output.empty()) { ThreadControlBlock::dispatch(); }
        size_t halt = 0x5555;
        size_t* volatile address = (size_t*) 0x100000;
        __asm__ volatile ("sw %[halt], 0(%[address])" : :[halt] "r" (halt), [address] "r" (address));
    }
}

