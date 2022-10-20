#ifndef __INCLUDE_MEMORY_MEMORY_HH__
#define __INCLUDE_MEMORY_MEMORY_HH__

#include "common/common.hh"

#include <iostream>
#include <unordered_map>

namespace sim {

constexpr Word kDummyWord = 0;

class Memory final {
private:
  struct MemoryStats {

    std::size_t numLoads;
    std::size_t numStores;
    std::size_t numPageFaults;

    MemoryStats() : numLoads(0), numStores(0), numPageFaults(0) {}
  };

  std::unordered_map<AddrType, Word> mem;
  MemoryStats stats;

  Word pageFaultHandle(AddrType addr);

public:
  Memory();
  Memory(const Memory &) = delete;
  Memory(Memory &&) = delete;
  Memory &operator=(const Memory &) = delete;
  Memory &operator=(Memory &&) = delete;

  Word loadWord(AddrType addr);
  void storeWord(AddrType addr, Word word);

  std::size_t getCurrMemSize() const;
  void printMemStats() const;
};

} // namespace sim

#endif // __INCLUDE_MEMORY_MEMORY_HH__
