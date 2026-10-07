/*
 * mem_common.cpp
 *
 * Helper functions that are shared by all three allocation strategies.
 */
#include <iostream>
#include <cstdlib>
#include <unistd.h>

#include "mem_common.h"

using std::cout;
using std::cerr;
using std::endl;

const std::size_t PARTITION_SIZES[NUM_PARTITIONS] = {32, 64, 128, 256, 512};

// total number of bytes requested from the OS using sbrk()
std::size_t heap_total = 0;

/* Error handling function: prints out error message and exits */
void print_error(std::string msg) {
    cerr << msg << endl;
    exit(EXIT_FAILURE);
}

std::size_t get_partition_size(std::size_t chunk_size) {
    if (chunk_size == 0) {
        return 0;
    }

    for (int i = 0; i < NUM_PARTITIONS; i++) {
        if (chunk_size <= PARTITION_SIZES[i]) {
            return PARTITION_SIZES[i];
        }
    }

    // bigger than the largest partition
    return 0;
}

int get_partition_index(std::size_t size) {
    for (int i = 0; i < NUM_PARTITIONS; i++) {
        if (PARTITION_SIZES[i] == size) {
            return i;
        }
    }
    return -1;
}

allocation *new_chunk(std::size_t size) {
    // sbrk() moves the program break to grow the heap. It returns the old
    // program break, which is the start of the new memory, or (void*) -1
    // if the heap could not be grown.
    // The address is a logical (virtual) address. The OS only gives the
    // page a physical frame when it is first used (demand paging).
    void *space = sbrk(size);
    if (space == (void*) -1) {
        print_error("ERROR: sbrk failed");
    }
    heap_total = heap_total + size;

    allocation *chunk = new allocation;
    chunk->size = size;
    chunk->used = 0;
    chunk->space = space;
    return chunk;
}

std::size_t get_heap_total() {
    return heap_total;
}

std::size_t get_internal_fragmentation(ChunkList &list) {
    std::size_t total = 0;

    ChunkList::iterator it;
    for (it = list.begin(); it != list.end(); it++) {
        // memory inside the partition that was not asked for
        total = total + ((*it)->size - (*it)->used);
    }
    return total;
}

// prints one chunk on a line: address, used size and chunk size
void print_chunk(allocation *chunk) {
    cout << chunk->space << "\t\t" << chunk->used << "\t\t"
         << chunk->size << endl;
}

void print_list(ChunkList &list) {
    cout << "Address\t\t\tUsed Size\tChunk Size" << endl;

    ChunkList::iterator it;
    for (it = list.begin(); it != list.end(); it++) {
        print_chunk(*it);
    }
    cout << endl;
}

void delete_list(ChunkList &list) {
    ChunkList::iterator it;
    for (it = list.begin(); it != list.end(); it++) {
        delete *it;
    }
    list.clear();
}
