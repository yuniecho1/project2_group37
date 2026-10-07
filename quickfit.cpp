/*
 * quickfit.cpp
 *
 * Quick fit allocation strategy.
 *
 * Instead of a single free list of different sized chunks, there is a
 * separate free list for each partition size (32, 64, 128, 256 and 512),
 * so there are 5 free lists and one allocated list. alloc() goes straight
 * to the free list for the size that is needed and takes the first chunk.
 *
 * With multiple lists of free partitions there is no searching, so quick
 * fit is very quick and every chunk it reuses is an exact fit for the
 * partition. First fit and best fit take O(n) in the worst case to find a
 * free chunk, but quick fit takes O(1).
 */
#include <iostream>
#include <list>

#include "allocator.h"
#include "mem_common.h"

using std::cout;
using std::endl;

// these are only used by the functions in this file
ChunkList allocated_list;               // the occupied chunks
ChunkList free_lists[NUM_PARTITIONS];   // one free list per partition size

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
        // no free chunk of this size, so ask the OS for more memory
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
    print_list(allocated_list);
    cout << "Internal fragmentation: "
         << get_internal_fragmentation(allocated_list) << " bytes" << endl;
    cout << endl;

    // print each of the free lists with the partition size it is for
    for (int i = 0; i < NUM_PARTITIONS; i++) {
        cout << "- Free Chunks (partition size " << PARTITION_SIZES[i]
             << ") -" << endl;
        print_list(free_lists[i]);
    }
}

void cleanup() {
    delete_list(allocated_list);
    for (int i = 0; i < NUM_PARTITIONS; i++) {
        delete_list(free_lists[i]);
    }
}
