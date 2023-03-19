#include "cache.h"
#include <cmath>
#include <iostream>
#include <string.h>

using std::cerr;
using std::cin;
using std::endl;
using std::getline;
using std::stoi;
using std::stoul;
using std::string;

int main(int argc, char *argv[]) {
  if (argc != 7) { // ensure that there are 7 command line arguments passed in
    cerr << "Argument count not valid" << endl; // otherwise throw error and
                                                // exit
    return 1;
  }

  // must check if the arguments passed for sets, blocks, and bytes are valid
  int num_sets = stoi(argv[1]);
  int num_blocks = stoi(argv[2]);
  int bytes = stoi(argv[3]);

  // check number of sets
  if (!(check_pow2(num_sets, 1))) {
    cerr << "Invalid number of sets" << endl;
    return 1;
  }

  // check number of blocks per set
  if (!(check_pow2(num_blocks, 1))) {
    cerr << "Invalid number of blocks per set" << endl;
    return 1;
  }

  // check number of bytes in each block (minimum is 4)
  if (!(check_pow2(bytes, 4))) {
    cerr << "Invalid number of bytes per block" << endl;
    return 1;
  }

  // integer constants based on inputs that are needed for execution
  int offsetBit = log2(bytes);
  int indexBit = log2(num_sets);

  int current_time = 0; // used for relative timing of different access
                        // operations, to determine evictions

  // check the qualitative parameters of the cache
  bool allocate = strcmp("write-allocate", argv[4]) == 0;
  bool write_back = strcmp("write-back", argv[5]) == 0;
  bool lru = strcmp("lru", argv[6]) == 0;

  // if the user combines no-write-allocate with write-back, that's invalid
  if (!allocate && write_back) {
    cerr << "Combining no-write-allocate with write-back is invalid" << endl;
    return 1;
  }

  // Use constructor to make a cache object using user's valid specifications
  Cache cache(num_sets, num_blocks, bytes, allocate, write_back, lru);

  // Now need to execute the lines of input from trace file one by one
  string trace_line; // temporary variable to hold each line of input
  unsigned int tag, idx, addr;
  while (getline(cin, trace_line)) {
    // grabs the string hex memory address from the line of trace, and converts
    // to unsigned int
    addr = stoul(trace_line.substr(2, 10), NULL, 16);
    // from the int address, grabs the index and tag bits respectively using bit
    // manipulation
    tag = addr >> (offsetBit + indexBit);
    idx = (addr >> offsetBit) & (num_sets - 1);
    char cur_inst = trace_line[0]; //"s" or "l" (ie. whether it is a store or
                                   // load instruction)

    if (cur_inst == 's') { // in the case of a store command
      cache.store(idx, tag, current_time);
    } else { // in the case of a load command
      cache.load(idx, tag, current_time);
    }

    current_time++; // increment the time, once per instruction (ie. line of
                    // trace input)
  }

  // output final metrics based on execution (the various stats belong to the
  // cache object itself)
  cache.printStats();
}