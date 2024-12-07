#pragma once
#include "../lib/hw.h"

class MemoryAllocator {
public:
    MemoryAllocator();
    MemoryAllocator(const MemoryAllocator&) = delete;

    static void create_memory_allocator();

    static void* kernel_malloc(size_t size);
    static int kernel_free(void* mem_block);

private:
    static size_t current_free_mem;
    struct FreeSegment{
        size_t segment_size;
        FreeSegment *next;
        FreeSegment *prev;
    };

    static FreeSegment* free_segment_head;

    static FreeSegment* best_fit(size_t size);
    static void add_free_segment(FreeSegment* prev_segment, size_t offset);
    static void merge_segments(FreeSegment* first, FreeSegment* second);
};

