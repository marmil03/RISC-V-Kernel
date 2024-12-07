#include "../h/Console.h"
#include "../h/RiscV.h"

Buffer IO::input;
Buffer IO::output;
Sem *IO::printer_sem;

int Buffer::put_char(char chr) {
    if(free_slots){
        buff[tail] = chr;
        tail = (tail + 1) % BUFFER_SIZE;
        free_slots--;
        Sem::sem_signal(item_available);
        return 0;
    }
    else return EOF;
}

char Buffer::get_char() {
    Sem::sem_wait(item_available);
    char chr = buff[head];
    head = (head + 1) % BUFFER_SIZE;
    free_slots++;
    Sem::sem_signal(space_available);
    return chr;
}

void Buffer::create_buffer() {
    Sem::sem_open(&item_available, 0);
    Sem::sem_open(&space_available, BUFFER_SIZE);
    head = tail = 0;
    free_slots = BUFFER_SIZE;
}

bool Buffer::empty() {
    return free_slots == BUFFER_SIZE;
}

void Buffer::delete_buffer() {
    sem_close(item_available);
}

void IO::create_io_system() {
    input.create_buffer();
    output.create_buffer();
    Sem::sem_open(&printer_sem, 1);
}

void IO::delete_io_system() {
    input.delete_buffer();
    output.delete_buffer();
    Sem::sem_close(printer_sem);
}

void IO::put_char(char c) {
    Sem::sem_wait(output.space_available);
    output.put_char(c);
}

char IO::get_char() {
    return input.get_char();
}

void printer_wrapper(void *){
    RiscV::printer();
}

void puts(const char* string){
    Sem::sem_wait(IO::printer_sem);
    int i = 0;
    while(string[i]!='\0'){
        putc(string[i++]);
    }
    Sem::sem_signal(IO::printer_sem);
}

void putint(size_t number){
    Sem::sem_wait(IO::printer_sem);
    size_t tmp = number, cnt = 1;
    while(number/10){
        cnt*=10;
        number/=10;
    }
    while(cnt){
        putc('0'+tmp/cnt);
        tmp = tmp - tmp/cnt*cnt;
        cnt/=10;
    }
    Sem::sem_signal(IO::printer_sem);
}