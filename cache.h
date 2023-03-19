#ifndef CACHE_H
#define CACHE_H

#include <iostream>
#include <unordered_map>
#include <vector>

struct Slot {
  int access_t, load_t;
  bool dirty;
};
typedef std::unordered_map<int, Slot>
    slot_set; // maps integer to a slot (this is a set of slots)

/*
 * Encapsulate all the cache functionalities in a C++ class
 */
class Cache {
private:
  // Initialize counters for statistics
  int num_loads = 0;
  int num_load_misses = 0;
  int num_load_hits = 0;
  int num_stores = 0;
  int num_store_hits = 0;
  int num_store_misses = 0;
  int num_cycles = 0;

  // Parameters for the cache (will take from the user input)
  int num_sets, num_blocks, bytes;
  bool allocate, write_back, lru;

  std::vector<slot_set> sets; // ie. an array of sets (this is the data
                              // structure for the full cache itself)

public:
  /*
   * This function outputs the final statistics of the simulation.
   */
  void printStats();

  /*
   * This function handles "stores"
   *   index: index of the block within the set
   *   tag: identifies the data block's tag
   *   cur_time: time of access (for eviction handling)
   */
  void store(unsigned int index, unsigned int tag, int cur_time);

  /*
   * This function handles "loads"
   *   index: index of the block within the set
   *   tag: identifies the data block's tag
   *   cur_time: time of access (for eviction handling)
   */
  void load(unsigned int index, unsigned int tag, int cur_time);

  /*
   * Pull a block from main memory and write into the cache. Used for cache
   * misses.
   *    set: address of the unordered set map
   *    tag: identifies the data block's tag
   *    cur_time: time of access (for eviction handling)
   */
  void load_cache_from_main(slot_set &set, unsigned int tag, int cur_time);

  Cache( // public constructor
      int num_sets, int num_blocks, int bytes, bool allocate, bool write_back,
      bool lru) {
    this->sets = std::vector<slot_set>(num_sets);
    this->num_sets = num_sets;
    this->num_blocks = num_blocks;
    this->bytes = bytes;
    this->allocate = allocate;
    this->write_back = write_back;
    this->lru = lru;
  };
};

/*
 * Return true if given number is a power of 2 and greater than minimum, else false. 
 * num: number to check 
 * minimum: smallest acceptable value
 */
bool check_pow2(int num, int minimum);
#endif