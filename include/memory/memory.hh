#ifndef __INCLUDE_MEMORY_MEMORY_HH__
#define __INCLUDE_MEMORY_MEMORY_HH__

#include "common/common.hh"
#include <iostream>
#include <list>
#include <unordered_map>

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

  Word pageFaultHandle(Addr addr);

public:
  Memory() = default;
  Memory(const Memory &) = delete;
  Memory(Memory &&) = delete;
  Memory &operator=(const Memory &) = delete;
  Memory &operator=(Memory &&) = delete;

  Word loadWord(Addr addr);
  void storeWord(Addr addr, Word word);

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

class PhysMemory {
public:
  struct Page {
    Page() = default;

  private:
    std::vector<Word> wordStorage{};
  };

  struct AddrSections {
    uint16_t index_pt1{};
    uint16_t index_pt2{};
    uint16_t offset{};

    AddrSections(uint16_t pt_1, uint16_t pt_2, uint16_t off)
        : index_pt1(pt_1), index_pt2(pt_2), offset(off) {}
    auto operator<=>(const AddrSections &) const = default;
  };

  PhysMemory() = default;
  AddrSections getVirtAddrSections(Addr addr);

private:
  using listIt = std::list<Page>::iterator;
  using PageTable =
      std::unordered_map<uint16_t, std::unordered_map<uint16_t, listIt>>;
  std::list<Page> pageStorage{};
  PageTable pageTable{};
};

} // namespace sim

#endif // __INCLUDE_MEMORY_MEMORY_HH__
