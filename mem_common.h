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

// the fixed partition sizes
#define NUM_PARTITIONS  5
#define MAX_PARTITION   512

// a linked list of chunks - each allocation is stored as its own pointer
typedef std::list<allocation*> ChunkList;

// the partition sizes in increasing order: 32, 64, 128, 256, 512
extern const std::size_t PARTITION_SIZES[NUM_PARTITIONS];

/* Error handling function: prints out error message and exits */
void print_error(std::string msg);

/*
 * Rounds a request up to the smallest partition size that can hold it
 * (with fixed partitions, a request is allocated the smallest partition
 * that is big enough).
 *   returns: the partition size, or 0 if the request is 0 or too big
 */
std::size_t get_partition_size(std::size_t chunk_size);

/*
 * Finds which partition (0 to NUM_PARTITIONS - 1) a partition size is.
 *   returns: the index, or -1 if size is not a partition size
 */
int get_partition_index(std::size_t size);

/*
 * Creates a new chunk by growing the heap using sbrk().
 *   returns: pointer to a new allocation struct for the chunk
 */
allocation *new_chunk(std::size_t size);

// total number of bytes requested from the OS using sbrk()
std::size_t get_heap_total();

/*
 * Adds up the internal fragmentation of the chunks in an allocated list.
 *   returns: total bytes that are inside a partition but not being used
 */
std::size_t get_internal_fragmentation(ChunkList &list);

// prints the address, used size and chunk size of each chunk in a list
// (the used size of a free chunk is 0)
void print_list(ChunkList &list);

// deletes all of the allocation structs in a list
void delete_list(ChunkList &list);

#endif // MEM_COMMON_H
