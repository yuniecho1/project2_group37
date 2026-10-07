/*
 * bestfit.cpp
 *
 * Best fit allocation strategy.
 *
 * Two linked lists are used to manage memory: a list of occupied chunks
 * and a list of free chunks (the holes). alloc() allocates the smallest
 * hole in the free list that is big enough.
 *
 * Best fit must search the entire free list, so it is slower than first
 * fit, but it produces the smallest leftover space in the chunk (the least
 * internal fragmentation).
 */
#include <iostream>
#include <list>

#include "allocator.h"
#include "mem_common.h"

using std::cout;
using std::endl;

// these are only used by the functions in this file
ChunkList allocated_list;   // the occupied chunks
ChunkList free_list;        // the free chunks

void *alloc(std::size_t chunk_size) {
    std::size_t size = get_partition_size(chunk_size);
    if (size == 0) {
        return NULL;
    }

    // best fit: allocate the smallest hole that is big enough
    // (must search the entire list, because it is not ordered by size)
    ChunkList::iterator best = free_list.end();
    ChunkList::iterator it;
    for (it = free_list.begin(); it != free_list.end(); it++) {
        if ((*it)->size >= size) {
            if (best == free_list.end() || (*it)->size < (*best)->size) {
                best = it;
            }
        }
    }

    allocation *chunk = NULL;
    if (best != free_list.end()) {
        chunk = *best;
        free_list.erase(best);
    }
    else {
        // no hole is big enough, so ask the OS for more memory
        chunk = new_chunk(size);
    }

    chunk->used = chunk_size;
    allocated_list.push_back(chunk);
    return chunk->space;
}

void dealloc(void *chunk) {
    // search for the pointer in the allocated list
    ChunkList::iterator it;
    for (it = allocated_list.begin(); it != allocated_list.end(); it++) {
        if ((*it)->space == chunk) {
            allocation *found = *it;
            found->used = 0;

            // move it to the end of the free list
            allocated_list.erase(it);
            free_list.push_back(found);
            return;
        }
    }

    // can't free memory that was never allocated
    print_error("ERROR: dealloc called on a chunk that was not allocated");
}

void print_strategy() {
    cout << "Best fit memory allocation" << endl;
}

void print_lists() {
    cout << "- Allocated Chunks -" << endl;
    print_list(allocated_list);
    cout << "Internal fragmentation: "
         << get_internal_fragmentation(allocated_list) << " bytes" << endl;
    cout << endl;

    cout << "- Free Chunks -" << endl;
    print_list(free_list);
}

void cleanup() {
    delete_list(allocated_list);
    delete_list(free_list);
}
