/*
 * firstfit.cpp
 *
 * First fit allocation strategy.
 *
 * Two linked lists are used to manage memory: a list of occupied chunks
 * and a list of free chunks (the holes). alloc() allocates the first hole
 * in the free list that is big enough.
 *
 * First fit is fast because it stops searching as soon as a hole is found,
 * but the hole can be much bigger than what is needed, which gives more
 * internal fragmentation.
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

    allocation *chunk = NULL;

    // first fit: allocate the first hole that is big enough
    ChunkList::iterator it = free_list.begin();
    while (chunk == NULL && it != free_list.end()) {
        if ((*it)->size >= size) {
            chunk = *it;
            free_list.erase(it);
        }
        else {
            it++;
        }
    }

    // no hole is big enough, so ask the OS for more memory
    if (chunk == NULL) {
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
    cout << "First fit memory allocation" << endl;
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
