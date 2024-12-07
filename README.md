# Multithreaded Operating System Kernel
## Overview

This project involves the implementation of a small but fully functional kernel of an operating system that supports multithreading with time sharing. The kernel includes memory allocation, thread management, semaphore support, asynchronous context switching, and preemption on timer and keyboard interrupts.
## Project Description

The kernel operates as a "library OS," where both the kernel and user application share the same address space. This design is tailored for embedded systems that execute a single program, preloaded into memory, rather than running arbitrary programs dynamically.
The project is implemented using RISC-V assembly and C/C++ and runs in a RISC-V emulator

## Key Features

- Memory Allocation: Dynamic continual memory allocation and deallocation.
- Thread Management: Create, manage, and terminate threads.
- Preemption and Time Sharing: Support for time-sharing and context switching.
- Semaphore Support: Create, manage and destroy semaphores.
- Asynchronous Context Switching: Triggered by internal timer and keyboard interrupts.

### Time-sharing, context switching and thread sleep.
![image](https://github.com/marmil03/RISC-V-Kernel/blob/main/Assets/Test%205.png)

### Semaphores for synchronization between threads

![image](https://github.com/marmil03/RISC-V-Kernel/blob/main/Assets/Test%207.png)
