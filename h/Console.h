#pragma once
#include "../lib/hw.h"
#include "Sem.h"
#include "syscall_cpp.hpp"

class Buffer{
public:
    void create_buffer();
    void delete_buffer();
    char get_char();
    int put_char(char chr);

    bool empty();
private:
    static const int BUFFER_SIZE = 300;
    int free_slots;
    int head, tail;
    char buff[BUFFER_SIZE];
    Sem *item_available, *space_available;
    friend class IO;
};

class IO {
public:
    static void create_io_system();
    static void delete_io_system();

    static void put_char(char c);
    static char get_char();

    static Buffer input, output;
    static Sem *printer_sem;
};

void printer_wrapper(void *);

void puts(const char* string);
void putint(size_t number);


