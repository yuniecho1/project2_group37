# COSC1114 Operating Systems Principles – Project 2: Memory Allocation

Group 37

A simulation of memory allocation in C++. It has the two functions
`void *alloc(std::size_t chunk_size)` and `void dealloc(void *chunk)`, which are
a simpler version of `malloc` and `free`, with three allocation strategies:
**First Fit**, **Best Fit** and **Quick Fit**. Memory is requested from the
operating system using `sbrk()` and is allocated strictly in fixed partition
sizes of **32, 64, 128, 256 and 512 bytes**.

## Tasks completed

| Task | Status |
| --- | --- |
| Task 1 – Solution design (two linked lists, `alloc` and `dealloc`) | Completed |
| Task 2 – Memory allocation (fixed partitions, `sbrk()` when no free chunk fits) | Completed |
| Task 3 – Allocation strategies: First Fit, Best Fit, Quick Fit | Completed (all three) |
| Task 4 – Memory deallocation (search the allocated list, fatal error if not found) | Completed |
| Makefile (`make` / `make all` builds the 3 programs, `make clean`), `-Wall -Werror` | Completed |
| Command line arguments and output | Completed |

## How to run (jupiter / saturn / titan.csit.rmit.edu.au)

```bash
unzip project2_group37.zip -d project2_group37
cd project2_group37
make                        # or: make all  (builds firstfit, bestfit, quickfit)

chmod +x p2_gen.sh
./p2_gen.sh 20 > datafile   # generate a sequence of 20 alloc and dealloc calls

./firstfit datafile         # First fit memory allocation
./bestfit datafile          # Best fit memory allocation
./quickfit datafile         # Quick fit memory allocation

make clean                  # gets rid of object and executable files
```

The programs must be run on Linux because they use `sbrk()`. They will not
work properly on MacOS.

## Files

| File | What it does |
| --- | --- |
| `allocator.h` | The `allocation` struct and the functions each strategy has |
| `firstfit.cpp` | First fit `alloc`/`dealloc`, with its allocated list and free list |
| `bestfit.cpp` | Best fit `alloc`/`dealloc`, with its allocated list and free list |
| `quickfit.cpp` | Quick fit `alloc`/`dealloc`, with its allocated list and 5 free lists |
| `mem_common.h`, `mem_common.cpp` | Helper functions shared by the strategies (partition sizes, `sbrk()`, printing) |
| `main.cpp` | Reads the datafile, calls `alloc`/`dealloc` and prints the lists |
| `Makefile` | Builds the three programs |
| `p2_gen.sh` | The provided datafile generator |

## Design

### Task 1 – Data structures

Each chunk of memory has an `allocation` struct, as suggested in the
specification, with one more field for the used size:

```cpp
struct allocation {
    std::size_t size;   // total size of the chunk (the partition size)
    std::size_t used;   // number of bytes the caller asked for (0 when free)
    void *space;        // start of the chunk, allocated using sbrk()
};
```

The linked lists are `std::list<allocation*>` from the standard library. Each
allocation is stored as its own pointer (not a list of structs), so moving a
chunk from one list to the other does not copy or destroy it.

The lists are global variables in each strategy file and are only used by the
functions in that file:

* First fit and best fit: one allocated list and one free list.
* Quick fit: one allocated list and five free lists (one for each partition
  size).

### Task 2 – Memory allocation

`alloc(chunk_size)` does the following:

1. Rounds the request up to the smallest partition size that can hold it
   (1–32 → 32, 33–64 → 64, 65–128 → 128, 129–256 → 256, 257–512 → 512).
   A request of 0 bytes or more than 512 bytes returns `NULL`.
2. Looks in the free list for a chunk, based on the strategy.
3. If no chunk is found, a new chunk of exactly the partition size is requested
   from the operating system using `sbrk()`. If `sbrk()` fails, an error is
   printed and the program exits.
4. The chunk is added to the allocated list and the pointer to the memory is
   returned.

### Task 3 – Allocation strategies

* **First fit** – the first chunk in the free list that is big enough is used.
* **Best fit** – the whole free list is searched for the chunk whose size is
  the closest match (the smallest chunk that is big enough).
* **Quick fit** – there is a separate free list for each partition size.
  `alloc` goes straight to the list for the size needed and takes the first
  chunk, so no searching is needed. If that list is empty the heap is grown,
  even if there is a bigger free chunk in another list.

### Task 4 – Memory deallocation

`dealloc(chunk)` searches the allocated list for the pointer passed in.

* If it is found, the chunk is removed from the allocated list and placed at
  the end of the free list (for quick fit, the free list for its size). The
  memory is not returned to the OS.
* If it is not found, this is a fatal error. An error message is printed and
  the program exits. This also happens if the same chunk is deallocated twice.

`dealloc` can free any chunk, like `free()`. The main program deallocates the
last chunk allocated (LIFO) because that is what the datafile means by
`dealloc`.

### Output

The lists are printed once at the end of the program, using the layout of the
sample in the project FAQ (address, used size and chunk size of each chunk):

```
First fit memory allocation
alloc calls: 6, dealloc calls: 3
total memory requested using sbrk: 928 bytes

- Allocated Chunks -
Address                 Used Size       Chunk Size
0x562925585000          500             512
0x5629255852a0          20              256
0x562925585200          120             128

Internal fragmentation: 256 bytes

- Free Chunks -
Address                 Used Size       Chunk Size
0x562925585280          0               32
```

The internal fragmentation is the total of (chunk size - used size) for the
allocated chunks. The used size of a free chunk is 0. For quick fit, each of the five free lists
is printed with the partition size it is for, e.g.
`- Free Chunks (partition size 32) -`.

The lists are only traversed for printing. Chunks are only added to or removed
from the lists by `alloc` and `dealloc`.

### Error handling

* Wrong number of arguments: prints a usage message and exits.
* The datafile cannot be opened: prints an error and exits.
* A line in the datafile that is not `alloc: N` or `dealloc`, an alloc size
  that is not 1 to 512, or a `dealloc` when nothing is allocated: prints a
  message with the line number and skips the line.
* `sbrk()` fails, or `dealloc` is given a pointer that was not allocated:
  prints an error and exits.

## Testing

All three programs were built and run on titan.csit.rmit.edu.au with datafiles
generated by `p2_gen.sh`. They were also run with
`valgrind --leak-check=full`, with no errors and no memory leaks.
