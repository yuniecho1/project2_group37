/*
 * main.cpp
 *
 * The main program for firstfit, bestfit and quickfit. It is compiled with
 * one of the strategy files, which has the alloc() and dealloc() functions.
 *
 * To compile: make
 *
 * To run: ./firstfit datafile     (or ./bestfit, ./quickfit)
 *
 * The datafile is made by p2_gen.sh and has one call on each line:
 *   alloc: 100     # allocate a chunk of 100 bytes
 *   dealloc        # deallocate the last chunk that was allocated (LIFO)
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>

#include "allocator.h"
#include "mem_common.h"

#define NUM_ARGS    2
#define FILL_CHAR   'x'     // written into each chunk to check it is usable

using std::cout;
using std::cerr;
using std::endl;

int main(int argc, char *argv[]) {
    if (argc != NUM_ARGS) {
        cerr << "usage: " << argv[0] << " datafile" << endl;
        return EXIT_FAILURE;
    }

    std::ifstream input(argv[1]);
    if (!input.is_open()) {
        cerr << "ERROR: could not open file " << argv[1] << endl;
        return EXIT_FAILURE;
    }

    // the chunks that are allocated at the moment, in the order they were
    // allocated, so the last one can be deallocated first
    std::vector<void*> chunks;

    int num_allocs = 0;
    int num_deallocs = 0;
    int line_num = 0;
    std::string line;

    // read in the file line-by-line
    while (std::getline(input, line)) {
        line_num++;

        std::istringstream iss(line);
        std::string command;
        iss >> command;

        if (command == "alloc:") {
            long chunk_size = 0;
            iss >> chunk_size;

            // check for bad values
            if (iss.fail() || chunk_size <= 0 || chunk_size > MAX_PARTITION) {
                cerr << "line " << line_num << ": invalid alloc size "
                     << "(must be 1 to " << MAX_PARTITION << "), skipped"
                     << endl;
            }
            else {
                void *chunk = alloc(chunk_size);
                if (chunk == NULL) {
                    cerr << "ERROR: alloc failed" << endl;
                    cleanup();
                    return EXIT_FAILURE;
                }

                // write to the chunk to check the memory can be used
                // (the first write to a new page causes a page fault and
                // the OS maps the page to a physical frame)
                memset(chunk, FILL_CHAR, chunk_size);
                chunks.push_back(chunk);
                num_allocs++;
            }
        }
        else if (command == "dealloc") {
            if (chunks.empty()) {
                cerr << "line " << line_num
                     << ": nothing to dealloc, skipped" << endl;
            }
            else {
                // dealloc the last chunk that was allocated
                dealloc(chunks.back());
                chunks.pop_back();
                num_deallocs++;
            }
        }
        else if (command != "") {
            cerr << "line " << line_num << ": unknown command " << command
                 << ", skipped" << endl;
        }
    }

    input.close();

    // print the results once at the end
    print_strategy();
    cout << "alloc calls: " << num_allocs
         << ", dealloc calls: " << num_deallocs << endl;
    cout << "total memory requested using sbrk: " << get_heap_total()
         << " bytes" << endl;
    cout << endl;

    print_lists();

    cleanup();
    return EXIT_SUCCESS;
}
