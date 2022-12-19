#ifndef __INCLUDE_MEMORY_MEMORY_HH__
#define __INCLUDE_MEMORY_MEMORY_HH__

#include <algorithm>
#include <iostream>
#include <unordered_map>

#include "common/common.hh"

namespace sim {

class Memory final {
public:
  struct MemoryStats {

    std::size_t numLoads{};
    std::size_t numStores{};
    std::size_t numPageFaults{};
  };

private:
  std::unordered_map<Addr, Word> mem{};
  MemoryStats stats{};
  bool isProgramStored{false};

  Word pageFaultHandle(Addr addr);

public:
  Memory() = default;
  Memory(const Memory &) = delete;
  Memory(Memory &&) = delete;
  Memory &operator=(const Memory &) = delete;
  Memory &operator=(Memory &&) = delete;

  Word loadWord(Addr addr);
  void storeWord(Addr addr, Word word);
  void setProgramStoredFlag() { isProgramStored = true; }

  template <std::forward_iterator It>
  void storeRange(Addr start, It begin, It end) {
    std::for_each(begin, end, [&start, this](auto curWord) {
      storeWord(start, curWord);
      start += kXLENInBytes;
    });
  }

  std::size_t getCurrMemSize() const;
  void printMemStats(std::ostream &ost) const;
  const MemoryStats &getMemStats() const;
};

} // namespace sim

#endif // __INCLUDE_MEMORY_MEMORY_HH__
