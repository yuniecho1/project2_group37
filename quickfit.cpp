/*
 * quickfit.cpp
 *
 * Quick fit allocation strategy: one free list per partition size (5 in
 * total). alloc() takes a chunk from the list for the requested partition
 * size, or grows the heap if that list is empty.
 */
#include <iostream>
#include <list>

#include "allocator.h"
#include "mem_common.h"

using std::cout;
using std::endl;

// static: these can only be used by the functions in this file
static ChunkList allocated_list;             // the occupied chunks
static ChunkList free_lists[NUM_PARTITIONS]; // one free list per partition size

void *alloc(std::size_t chunk_size) {
    std::size_t size = get_partition_size(chunk_size);
    if (size == 0) {
        return NULL;
    }

    // quick fit: only look in the free list for this partition size
    int index = get_partition_index(size);
    allocation *chunk = NULL;

    if (!free_lists[index].empty()) {
        // every chunk in this list is the right size, so take the first
        chunk = free_lists[index].front();
        free_lists[index].pop_front();
    }
    else {
        // no free chunk of this size, so grow the heap
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

            // move it to the end of the free list for its partition size
            int index = get_partition_index(found->size);
            allocated_list.erase(it);
            free_lists[index].push_back(found);
            return;
        }
    }

    // can't free memory that was never allocated
    print_error("ERROR: dealloc called on a chunk that was not allocated");
}

void print_strategy() {
    cout << "Quick fit memory allocation" << endl;
}

void print_lists() {
    cout << "- Allocated Chunks -" << endl;
    print_list(allocated_list, true);
    cout << "Internal fragmentation: "
         << get_internal_fragmentation(allocated_list) << " bytes" << endl;
    cout << endl;

    // print each of the free lists with the partition size it is for
    for (int i = 0; i < NUM_PARTITIONS; i++) {
        cout << "- Free Chunks (partition size " << PARTITION_SIZES[i]
             << ") -" << endl;
        print_list(free_lists[i], false);
    }
}

void cleanup() {
    delete_list(allocated_list);
    for (int i = 0; i < NUM_PARTITIONS; i++) {
        delete_list(free_lists[i]);
    }
}
