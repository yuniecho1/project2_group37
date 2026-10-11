/*
 * mem_common.h
 *
 * Helper functions that are shared by all three allocation strategies.
 */
#ifndef MEM_COMMON_H
#define MEM_COMMON_H

#include <cstddef>
#include <list>
#include <string>

#include "allocator.h"

// the number of fixed partition sizes - PARTITION_SIZES in mem_common.cpp
// must have exactly this many values, in increasing order
#define NUM_PARTITIONS  5

// stores pointers, so removing a node does not destroy the allocation
typedef std::list<allocation*> ChunkList;

// the partition sizes in increasing order: 32, 64, 128, 256, 512
extern const std::size_t PARTITION_SIZES[NUM_PARTITIONS];

// the largest partition size, taken from PARTITION_SIZES
#define MAX_PARTITION   (PARTITION_SIZES[NUM_PARTITIONS - 1])

// prints an error message and exits the program
void print_error(std::string msg);

/*
 * Rounds a request up to the smallest partition size that can hold it.
 *   returns: the partition size, or 0 if the request is 0 or too big
 */
std::size_t get_partition_size(std::size_t chunk_size);

/*
 * Finds the index (0 to NUM_PARTITIONS - 1) of a partition size.
 *   returns: the index, or -1 if size is not a partition size
 */
int get_partition_index(std::size_t size);

/*
 * Grows the heap by size bytes using sbrk(). Exits if sbrk() fails.
 *   returns: pointer to a new allocation struct for the chunk
 */
allocation *new_chunk(std::size_t size);

// total number of bytes requested from the OS using sbrk()
std::size_t get_heap_total();

// returns the total internal fragmentation (size - used) of a list

std::size_t get_internal_fragmentation(ChunkList &list);

// prints the address and chunk size of each chunk in a list, and the used
// size if show_used is true (allocated list only)
void print_list(ChunkList &list, bool show_used);

// deletes all of the allocation structs in a list
void delete_list(ChunkList &list);

#endif // MEM_COMMON_H
