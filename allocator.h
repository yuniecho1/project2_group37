/*
 * allocator.h
 *
 * COSC1114 Operating Systems Principles - Project 2: Memory Allocation
 *
 * The functions that every allocation strategy (first fit, best fit and
 * quick fit) provides. Each strategy is in its own .cpp file. The main
 * program (main.cpp) is compiled with one strategy file to make each program.
 */
#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <cstddef>

/*
 * The accounting information kept for each chunk of memory.
 * Memory is allocated in fixed partitions, so a chunk is always one of the
 * partition sizes (32, 64, 128, 256 or 512 bytes). The difference between
 * size and used is the internal fragmentation of the chunk: memory that is
 * internal to the partition but not being used.
 */
struct allocation {
    std::size_t size;   // total size of the chunk (the partition size)
    std::size_t used;   // number of bytes the caller asked for (0 when free)
    void *space;        // start of the chunk, allocated using sbrk()
};

/*
 * Allocates a chunk that can hold chunk_size bytes, rounded up to a
 * partition size. Uses a free chunk if one fits, otherwise grows the heap
 * with sbrk().
 *   returns: pointer to the chunk, or NULL if chunk_size is 0 or larger
 *            than the largest partition
 */
void *alloc(std::size_t chunk_size);

/*
 * Moves a chunk from the allocated list to the end of the free list.
 * Fatal error (program exits) if the chunk is not in the allocated list.
 */
void dealloc(void *chunk);

// prints the name of the allocation strategy
void print_strategy();

// prints the allocated list and the free list(s)
void print_lists();

// deletes the allocation structs at the end of the program
void cleanup();

#endif // ALLOCATOR_H
