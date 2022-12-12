#ifndef __INCLUDE_MEMORY_MEMORY_HH__
#define __INCLUDE_MEMORY_MEMORY_HH__

#include "common/common.hh"
#include <algorithm>
#include <concepts>
#include <iostream>
#include <list>
#include <memory>
#include <unordered_map>
#include <vector>

namespace sim {

class PhysMemory final {
public:
  struct Page {
    Page() { wordStorage.resize(kPageSize / sizeof(Word)); }

    std::vector<Word> wordStorage{};
  };

  struct AddrSections {
    uint16_t index_pt1{};
    uint16_t index_pt2{};
    uint16_t offset{};

    AddrSections(uint16_t pt_1, uint16_t pt_2, uint16_t off)
        : index_pt1(pt_1), index_pt2(pt_2), offset(off) {}
    AddrSections(Addr addr) {
      constexpr std::pair<uint8_t, uint8_t> of_bits{11, 0};
      constexpr std::pair<uint8_t, uint8_t> pt2_bits{21, 12};
      constexpr std::pair<uint8_t, uint8_t> pt1_bits{31, 22};
      index_pt1 =
          static_cast<uint16_t>(getBits<pt1_bits.first, pt1_bits.second>(addr));
      index_pt2 =
          static_cast<uint16_t>(getBits<pt2_bits.first, pt2_bits.second>(addr));
      offset =
          static_cast<uint16_t>(getBits<of_bits.first, of_bits.second>(addr));
    }
    bool operator==(const AddrSections &) const = default;
  };

  enum struct MemoryOp { STORE = 0, LOAD = 1 };

  using listIt = std::list<Page>::iterator;
  using PTLowLvl = std::unordered_map<uint16_t, listIt>;
  using PTHighLvl = std::unordered_map<uint16_t, PTLowLvl>;

  PhysMemory() = default;
  listIt pageTableLookup(const AddrSections &sect, MemoryOp op);
  template <typename T> T *getEntity(Addr addr, MemoryOp op);
  uint16_t getOffset(Addr addr);

private:
  std::list<Page> pageStorage{};
  PTHighLvl pageTable{};
};

class Memory final {
public:
  struct MemoryStats {

    std::size_t numLoads{};
    std::size_t numStores{};
  };

private:
  MemoryStats stats{};
  PhysMemory physMem{};

public:
  Memory() = default;
  Memory(const Memory &) = delete;
  Memory(Memory &&) = delete;
  Memory &operator=(const Memory &) = delete;
  Memory &operator=(Memory &&) = delete;

  Word loadWord(Addr addr);
  void storeWord(Addr addr, Word word);
  Half loadHalf(Addr addr);
  void storeHalf(Addr addr, Half half);
  Byte loadByte(Addr addr);
  void storeByte(Addr addr, Byte Byte);

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
/*
class TLB
{
  struct TLBEntry
  {
    Addr VirtualAddress{};
  }
};
*/

template <typename T> inline T *PhysMemory::getEntity(Addr addr, MemoryOp op) {
  if (addr % sizeof(T) || ((getOffset(addr) + sizeof(T)) > (1 << kOffsetBits)))
    throw std::runtime_error("Misaligned memory access is not supported!");
  AddrSections sections(addr);
  auto offset = sections.offset;
  auto it = PhysMemory::pageTableLookup(sections, op);
  Word *word = &it->wordStorage.at(offset / sizeof(Word));
  Byte *byte = reinterpret_cast<Byte *>(word) + (offset % sizeof(Word));
  return reinterpret_cast<T *>(byte);
}

inline Word Memory::loadWord(Addr addr) {
  stats.numLoads++;
  Word loadedWord = *physMem.getEntity<Word>(addr, PhysMemory::MemoryOp::LOAD);

  return loadedWord;
}

inline void Memory::storeWord(Addr addr, Word word) {
  stats.numStores++;
  *physMem.getEntity<Word>(addr, PhysMemory::MemoryOp::STORE) = word;
}

inline Half Memory::loadHalf(Addr addr) {
  stats.numLoads++;
  Half loadedWord = *physMem.getEntity<Half>(addr, PhysMemory::MemoryOp::LOAD);

  return loadedWord;
}

inline void Memory::storeHalf(Addr addr, Half half) {
  stats.numStores++;
  *physMem.getEntity<Half>(addr, PhysMemory::MemoryOp::STORE) = half;
}

inline Byte Memory::loadByte(Addr addr) {
  stats.numLoads++;
  Byte loadedWord = *physMem.getEntity<Byte>(addr, PhysMemory::MemoryOp::LOAD);

  return loadedWord;
}

inline void Memory::storeByte(Addr addr, Byte byte) {
  stats.numStores++;
  *physMem.getEntity<Byte>(addr, PhysMemory::MemoryOp::STORE) = byte;
}

} // namespace sim

#endif // __INCLUDE_MEMORY_MEMORY_HH__
