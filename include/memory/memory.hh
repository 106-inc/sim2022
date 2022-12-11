#ifndef __INCLUDE_MEMORY_MEMORY_HH__
#define __INCLUDE_MEMORY_MEMORY_HH__

#include "common/common.hh"
#include <iostream>
#include <list>
#include <memory>
#include <unordered_map>

namespace sim {

class PhysMemory;

class Memory final {
public:
  struct MemoryStats {

    std::size_t numLoads{};
    std::size_t numStores{};
  };

private:
  MemoryStats stats{};
  std::unique_ptr<PhysMemory> physMem{};

public:
  Memory() : physMem(std::make_unique<PhysMemory>()) {}
  Memory(const Memory &) = delete;
  Memory(Memory &&) = delete;
  Memory &operator=(const Memory &) = delete;
  Memory &operator=(Memory &&) = delete;

  Word loadWord(Addr addr);
  void storeWord(Addr addr, Word word);

  void printMemStats(std::ostream &ost) const;
  const MemoryStats &getMemStats() const;

  template <std::forward_iterator It>
  void storeRange(Addr start, It begin, It end) {
    std::for_each(begin, end, [&start, this](auto curWord) {
      storeWord(start, curWord);
      start += kXLENInBytes;
    });
  }
};

class PhysMemory final {
public:
  struct Page {
    Page() = default;

    std::array<Word, kPageSize / sizeof(Word)> wordStorage{};
  };

  struct AddrSections {
    uint16_t index_pt1{};
    uint16_t index_pt2{};
    uint16_t offset{};

    AddrSections(uint16_t pt_1, uint16_t pt_2, uint16_t off)
        : index_pt1(pt_1), index_pt2(pt_2), offset(off) {}
    auto operator<=>(const AddrSections &) const = default;
  };

  enum struct MemoryOp { STORE = 0, LOAD = 1 };

  using listIt = std::list<Page>::iterator;
  using PTLowLvl = std::unordered_map<uint16_t, listIt>;
  using PTHighLvl = std::unordered_map<uint16_t, PTLowLvl>;

  PhysMemory() = default;
  AddrSections getVirtAddrSections(Addr addr);
  listIt pageTableLookup(const AddrSections &sect, MemoryOp op);
  Word &getWord(Addr addr, MemoryOp op);

private:
  std::list<Page> pageStorage{};
  PTHighLvl pageTable{};
};

} // namespace sim

#endif // __INCLUDE_MEMORY_MEMORY_HH__
