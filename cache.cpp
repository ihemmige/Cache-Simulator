#include "cache.h"
#include <limits>

using std::cout;
using std::endl;
using std::pair;

/*
 * This function outputs the final statistics of the simulation, using instance
 * variables of Cache.
 */
void Cache::printStats() {
  cout << "Total loads: " << num_loads << endl;
  cout << "Total stores: " << num_stores << endl;
  cout << "Load hits: " << num_load_hits << endl;
  cout << "Load misses: " << num_load_misses << endl;
  cout << "Store hits: " << num_store_hits << endl;
  cout << "Store misses: " << num_store_misses << endl;
  cout << "Total cycles: " << num_cycles << endl;
}

/*
 * This function handles "stores"
 *   index: index of the block within the set
 *   tag: identifies the data block's tag
 *   cur_time: time of access (for eviction handling)
 */
void Cache::store(unsigned int index, unsigned int tag, int cur_time) {
  num_stores++; // increment total count of stores

  if (sets[index].count(tag)) { // in the case of a store hit
    num_store_hits++;           // increment store hits variable
    sets[index][tag].access_t =
        cur_time;       // update the most recently accessed time for this block
    num_cycles += 1;    // 1 cycle for the write to cache
  } else {              // in the case of a store miss
    num_store_misses++; // increment store misses variable
    if (allocate) { // if write allocate cache, then this block will be loaded
                    // into cache
      load_cache_from_main(sets[index], tag, cur_time);
      num_cycles += 25 * bytes; // 100 clock cycles for every 4 bytes
    }
    num_cycles += 1; // 1 cycle for the write into the cache after the value has
                     // been loaded
  }
  if (write_back) {
    sets[index][tag].dirty =
        write_back; // if write-back cache, the block is marked as dirty since
                    // main memory is not updated here
  } else {
    num_cycles += 100; // if not write-back, every write goes straight to
                       // memory, so that adds to cycle count
  }
}

/*
 * Return true if number is power of 2, greater than minimum.
 * num: number to check
 * minimum: smallest acceptable value
 */
bool check_pow2(int num, int minimum) {
  if (num < minimum)
    return false;
  return !(num & (num - 1));
}

/*
 * This function handles "loads"
 *   index: index of the block within the set
 *   tag: identifies the data block's tag
 *   cur_time: time of access (for eviction handling)
 */
void Cache::load(unsigned int index, unsigned int tag, int cur_time) {
  num_loads++;                            // increment total count of loads
  if (sets[index].count(tag)) {           // in the case of a load hit
    num_load_hits++;                      // increment count of load hits
    sets[index][tag].access_t = cur_time; // update the most recent access time
    num_cycles++;      // 1 clock cycle for the access to cache for load
  } else {             // in the case of a load miss
    num_load_misses++; // increment count of load misses
    load_cache_from_main(sets[index], tag,
                         cur_time); // load from the main memory into cache
    num_cycles +=
        (25 * bytes); // 100 cycles for every 4 bytes loaded from memory
    num_cycles += 1;  // 1 clock cycle for the read from cache
  }
}

/*
 * Pull a block from main memory and write into the cache. Used for cache
 * misses.
 *    set: address of the unordered set map
 *    tag: identifies the data block's tag
 *    cur_time: time of access (for eviction handling)
 */
void Cache::load_cache_from_main(slot_set &set, unsigned int tag,
                                 int cur_time) {
  if ((int)set.size() == num_blocks) { // if the set is already full, we need to
                                       // first evict something else
    // variables for the slot we are going to evict
    int evict_tag = -1;
    int evict_time =
        std::numeric_limits<int>::max(); // set as max because any time will
                                         // then be lesser

    for (pair<int, Slot> slot :
         set) { // iterate through all the existing slots in the set

      if (!lru &&
          evict_time >
              slot.second.load_t) { // for a FIFO cache, we are looking for the
                                    // slot with the earliest load time
        evict_time = slot.second.load_t;
        evict_tag = slot.first;
      }
      if (lru && evict_time >
                     slot.second.access_t) { // for a LRU cache, we are looking
                                             // for the slot with the smallest
                                             // (ie. least recent) access time
        evict_time = slot.second.access_t;
        evict_tag = slot.first;
      }
    }
    if (set[evict_tag].dirty) {
      num_cycles +=
          25 * bytes; // if the block being evicted is dirty, then we take 100
                      // cycles for every 4 bytes to load back into main memory
    }
    set.erase(evict_tag); // remove that slot from the set
  }
  set[tag] = {cur_time, cur_time,
              false}; // initialize the newly loaded block with updated times
}
