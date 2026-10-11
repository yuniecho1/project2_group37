/*
 * main.cpp
 *
 * Usage: ./firstfit datafile   (or ./bestfit, ./quickfit)
 *
 * Runs the calls in a datafile made by p2_gen.sh, one per line:
 *   alloc: N   allocate N bytes
 *   dealloc    deallocate the last chunk allocated (LIFO)
 * then prints the allocated and free lists once at the end.
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

    // currently allocated chunks, most recent last (for LIFO dealloc)
    std::vector<void*> chunks;

    int num_allocs = 0;
    int num_deallocs = 0;
    int line_num = 0;
    std::string line;

    while (std::getline(input, line)) {
        line_num++;

        std::istringstream iss(line);
        std::string command;
        iss >> command;

        if (command == "alloc:") {
            long chunk_size = 0;
            iss >> chunk_size;
            bool bad_number = iss.fail();

            // anything left after the number (e.g. "alloc: 12abc") is invalid
            std::string extra;
            if (!bad_number && iss >> extra) {
                bad_number = true;
            }

            // check for bad values (chunk_size <= 0 is checked first, so the
            // cast to std::size_t below is safe)
            if (bad_number || chunk_size <= 0
                    || (std::size_t) chunk_size > MAX_PARTITION) {
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

                // write to the chunk to check the memory is usable
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

    // getline() stops at the end of the file, but bad() means the file
    // could not be read (e.g. the datafile is a directory)
    if (input.bad()) {
        cerr << "ERROR: could not read file " << argv[1] << endl;
        cleanup();
        return EXIT_FAILURE;
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
