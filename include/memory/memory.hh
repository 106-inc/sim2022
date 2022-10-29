#ifndef __INCLUDE_MEMORY_MEMORY_HH__
#define __INCLUDE_MEMORY_MEMORY_HH__

#include "common/common.hh"
#include <iostream>
#include <unordered_map>

namespace sim {

class Memory final {
public:
  struct MemoryStats {

    std::size_t numLoads;
    std::size_t numStores;
    std::size_t numPageFaults;

    MemoryStats() : numLoads(0), numStores(0), numPageFaults(0) {}
  };

private:
  std::unordered_map<Addr, Word> mem;
  MemoryStats stats;

  Word pageFaultHandle(Addr addr);

public:
  Memory();
  Memory(const Memory &) = delete;
  Memory(Memory &&) = delete;
  Memory &operator=(const Memory &) = delete;
  Memory &operator=(Memory &&) = delete;

  Word loadWord(Addr addr);
  void storeWord(Addr addr, Word word);

  std::size_t getCurrMemSize() const;
  void printMemStats(std::ostream &ost) const;
  const MemoryStats &getMemStats() const;
};

} // namespace sim

#endif // __INCLUDE_MEMORY_MEMORY_HH__
