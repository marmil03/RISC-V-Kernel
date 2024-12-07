.global _ZN5RiscV4pushEv
.type _ZN5RiscV4pushEv, @function
_ZN5RiscV4pushEv:
    addi sp, sp, -256
    .irp idx, 3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
        sd x\idx, \idx * 8(sp)
    .endr
    ret

.global _ZN5RiscV3popEv
.type _ZN5RiscV3popEv, @function
_ZN5RiscV3popEv:
    .irp idx, 3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
        ld x\idx, \idx * 8(sp)
    .endr
    addi sp, sp, 256
    ret

.global _ZN18ThreadControlBlock14context_switchEPNS_7ContextES1_
.type _ZN18ThreadControlBlock14context_switchEPNS_7ContextES1_, @function
_ZN18ThreadControlBlock14context_switchEPNS_7ContextES1_:
    sd ra, 0*8(a0)
    sd sp, 1*8(a0)

    ld ra, 0*8(a1)
    ld sp, 1*8(a1)

    ret


.extern _ZN5RiscV23supervisor_trap_handlerEv

.align 4
.global _ZN5RiscV15supervisor_trapEv
.type _ZN5RiscV15supervisor_trapEv, @function
_ZN5RiscV15supervisor_trapEv:
        addi sp, sp, -256
        .irp idx, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
            sd x\idx, \idx * 8(sp)
        .endr

        call _ZN5RiscV23supervisor_trap_handlerEv

        .irp idx, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
            ld x\idx, \idx * 8(sp)
        .endr
        addi sp, sp, 256

        sret