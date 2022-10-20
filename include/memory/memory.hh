#ifndef __INCLUDE_MEMORY_MEMORY_HH__
#define __INCLUDE_MEMORY_MEMORY_HH__

#include "common/common.hh"

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
  Word load(std::size_t);
};

} // namespace sim

#endif // __INCLUDE_MEMORY_MEMORY_HH__
