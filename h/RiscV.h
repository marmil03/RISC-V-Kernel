#pragma once
#include "../lib/hw.h"

class RiscV{
public:
    static void set_user_mode(bool status);
    static bool is_user_mode();

    static void push();
    static void pop();

    static void pop_spp_spie();

    [[noreturn]] static void printer();

    static size_t read_scause();
    static void write_scause(size_t scause);

    static size_t read_sepc();
    static void write_sepc(size_t sepc);

    static size_t read_stvec();
    static void write_stvec(size_t stvec);

    static size_t read_stval();
    static void write_stval(size_t stval);

    enum BitMaskSip{
        SIP_SSIP = (1 << 1),
        SIP_STIP = (1 << 5),
        SIP_SEIP = (1 << 9)
    };

    static void mask_set_sip(size_t mask);
    static void mask_clear_sip(size_t mask);

    static size_t read_sip();
    static void write_sip(size_t sip);

    enum BitMaskStatus{
        SSTATUS_SIE = (1 << 1),
        SSTATUS_SPIE = (1 << 5),
        SSTATUS_SPP = (1 << 8)
    };

    static void mask_set_sstatus(size_t mask);
    static void mask_clear_sstatus(size_t mask);

    static size_t read_sstatus();
    static void write_sstatus(size_t sstatus);

    static void supervisor_trap();
private:
    static bool user_mode;

    enum InterruptAndExceptionCodes{
        USER_SOFTWARE_INTERRUPT = 0x8000000000000000UL,
        SUPERVISOR_SOFTWARE_INTERRUPT = 0x8000000000000001UL,
        SUPERVISOR_EXTERNAL_INTERRUPT = 0x8000000000000009UL,
        ECALL_FROM_USER = 0x0000000000000008UL,
        ECALL_FROM_SUPERVISOR = 0x0000000000000009UL
    };

    static void supervisor_trap_handler();
};

 inline size_t RiscV::read_scause() {
    size_t volatile scause;
    __asm__ volatile ("csrr %[scause], scause": [scause] "=r"(scause));
    return scause;
}

inline void RiscV::write_scause(size_t scause) {
    __asm__ volatile("csrw scause, %[scause]": :[scause] "r"(scause):"memory");
}

inline size_t RiscV::read_sepc() {
    size_t volatile sepc;
    __asm__ volatile("csrr %[sepc], sepc": [sepc] "=r"(sepc));
    return sepc;
}

inline void RiscV::write_sepc(size_t sepc) {
    __asm__ volatile("csrw sepc, %[sepc]": :[sepc] "r"(sepc));
}

inline size_t RiscV::read_stvec() {
    size_t volatile stvec;
    __asm__ volatile("csrr %[stvec], stvec": [stvec] "=r"(stvec));
    return stvec;
}

inline void RiscV::write_stvec(size_t stvec) {
    __asm__ volatile("csrw stvec, %[stvec]": :[stvec] "r"(stvec));
}

inline size_t RiscV::read_stval() {
    size_t volatile stval;
    __asm__ volatile("csrr %[stval], stval": [stval] "=r"(stval));
    return stval;
}

inline void RiscV::write_stval(size_t stval) {
    __asm__ volatile("csrw stval, %[stval]": :[stval] "r"(stval));
}

inline void RiscV::mask_set_sip(size_t mask) {
    __asm__ volatile("csrs sip, %[mask]": :[mask] "r"(mask));
}

inline void RiscV::mask_clear_sip(size_t mask) {
    __asm__ volatile("csrc sip, %[mask]": :[mask] "r"(mask));
}

inline size_t RiscV::read_sip() {
    size_t volatile sip;
    __asm__ volatile("csrr %[sip], sip": [sip] "=r"(sip));
    return sip;
}

inline void RiscV::write_sip(size_t sip) {
    __asm__ volatile("csrw sip, %[sip]": :[sip] "r"(sip));
}

inline void RiscV::mask_set_sstatus(size_t mask) {
    __asm__ volatile("csrs sstatus, %[mask]": : [mask] "r"(mask));
}

inline void RiscV::mask_clear_sstatus(size_t mask) {
    __asm__ volatile("csrc sstatus, %[mask]": : [mask] "r"(mask));
}

inline size_t RiscV::read_sstatus() {
    size_t volatile sstatus;
    __asm__ volatile("csrr %[sstatus], sstatus": [sstatus] "=r"(sstatus));
    return sstatus;
}

inline void RiscV::write_sstatus(size_t sstatus) {
    __asm__ volatile("csrw sstatus, %[sstatus]": :[sstatus] "r"(sstatus));
}
