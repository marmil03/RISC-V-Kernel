#include "../h/MemoryAllocator.h"
#include "../lib/console.h"
#include "../lib/mem.h"

size_t MemoryAllocator::current_free_mem = 0;
MemoryAllocator::FreeSegment* MemoryAllocator::free_segment_head = nullptr;

MemoryAllocator::MemoryAllocator() = default;

void MemoryAllocator::create_memory_allocator() {
    if(free_segment_head == nullptr){
        free_segment_head = (FreeSegment*)HEAP_START_ADDR;
        while(((size_t)free_segment_head) % MEM_BLOCK_SIZE != 0) free_segment_head = (FreeSegment*)((char*)free_segment_head + 1); // ALIGN MEM
        current_free_mem = (char*)HEAP_END_ADDR - (char*)free_segment_head - MEM_BLOCK_SIZE; // TOTAL MEM SIZE
        free_segment_head->next = free_segment_head->prev = nullptr;
        free_segment_head->segment_size = current_free_mem;
    }
}

void* MemoryAllocator::kernel_malloc(size_t size) {
    size = (size + MEM_BLOCK_SIZE - 1)/MEM_BLOCK_SIZE * MEM_BLOCK_SIZE; // Only required memory without descriptor

    if(current_free_mem < size) return nullptr; // Out of memory
    FreeSegment* new_mem_block = best_fit(size);

    add_free_segment(new_mem_block, size);

    new_mem_block->segment_size = size;
    new_mem_block->next = nullptr;
    new_mem_block->prev = nullptr;

    return (void*) ((char*)new_mem_block + MEM_BLOCK_SIZE);
}

int MemoryAllocator::kernel_free(void *mem_block) {
    if(mem_block == nullptr) return -1;
    FreeSegment* current_segment = free_segment_head;
    while(current_segment->next && (char*)current_segment->next < (char*)mem_block){
        current_segment = current_segment->next;
    }
    FreeSegment* new_segment = (FreeSegment*)((char*)mem_block - MEM_BLOCK_SIZE);
    current_free_mem+=new_segment->segment_size;
    if((char*)mem_block < (char*)free_segment_head){
        new_segment->next = current_segment;
        current_segment->prev = new_segment;
        free_segment_head = new_segment;

        merge_segments(new_segment, current_segment);
    }
    else {
        new_segment->next = current_segment->next;
        new_segment->prev = current_segment;
        current_segment->next->prev = new_segment;
        current_segment->next = new_segment;

        merge_segments(new_segment, new_segment->next);
        merge_segments(current_segment, new_segment);
    }

    mem_block = nullptr;
    return 0;
}

MemoryAllocator::FreeSegment *MemoryAllocator::best_fit(size_t size) {
    FreeSegment* current_segment = free_segment_head, *best_segment = nullptr;
    while(current_segment) {
        if (current_segment->segment_size >= size && (!best_segment || current_segment->segment_size < best_segment->segment_size)) {
            best_segment = current_segment;
        }
        current_segment = current_segment->next;
    }
    return best_segment;
}

void MemoryAllocator::add_free_segment(MemoryAllocator::FreeSegment *segment, size_t size) {
    size_t old_size = segment->segment_size;
    FreeSegment* old_next = segment->next, *old_prev = segment->prev;
    if(segment == free_segment_head){
        if(old_size == size) {
            free_segment_head = free_segment_head->next;
            free_segment_head->prev = nullptr;
            current_free_mem -= size;
            return;
        }
        free_segment_head = (FreeSegment*)((char *)free_segment_head + MEM_BLOCK_SIZE + size);
        free_segment_head->next = old_next;
        free_segment_head->prev = old_prev;
        free_segment_head->segment_size = old_size - size - MEM_BLOCK_SIZE;
        current_free_mem -= (size + MEM_BLOCK_SIZE);
    }
    else{
        if(old_size == size){
            segment->next->prev = segment->prev;
            segment->prev->next = segment->next;
            current_free_mem -= size;
        }
        else {
            FreeSegment *new_seg = (FreeSegment *) ((char *) segment + MEM_BLOCK_SIZE + size);
            new_seg->prev = old_prev;
            new_seg->next = old_next;
            if(old_next) old_next->prev = new_seg;
            old_prev->next = new_seg;
            new_seg->segment_size = old_size - size - MEM_BLOCK_SIZE;
            current_free_mem -= (size + MEM_BLOCK_SIZE);
        }
    }
}

void MemoryAllocator::merge_segments(MemoryAllocator::FreeSegment *first, MemoryAllocator::FreeSegment *second) {
    if((char*)first + first->segment_size + MEM_BLOCK_SIZE == (char*)second){
        first->segment_size += MEM_BLOCK_SIZE + second->segment_size;
        first->next = second->next;
        if(second->next) second->next->prev = first;
        second->next = nullptr;
        second->segment_size = 0;
        current_free_mem += MEM_BLOCK_SIZE;
    }
};
