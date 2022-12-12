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
    uint16_t indexPt1{};
    uint16_t indexPt2{};
    uint16_t offset{};

    AddrSections(uint16_t pt_1, uint16_t pt_2, uint16_t off)
        : indexPt1(pt_1), indexPt2(pt_2), offset(off) {}
    AddrSections(Addr addr) {
      constexpr std::pair<uint8_t, uint8_t> of_bits{11, 0};
      constexpr std::pair<uint8_t, uint8_t> pt2_bits{21, 12};
      constexpr std::pair<uint8_t, uint8_t> pt1_bits{31, 22};
      indexPt1 =
          static_cast<uint16_t>(getBits<pt1_bits.first, pt1_bits.second>(addr));
      indexPt2 =
          static_cast<uint16_t>(getBits<pt2_bits.first, pt2_bits.second>(addr));
      offset =
          static_cast<uint16_t>(getBits<of_bits.first, of_bits.second>(addr));
    }
    bool operator==(const AddrSections &) const = default;
  };

  class PageFaultException : public std::runtime_error {
  public:
    PageFaultException(const char *msg) : std::runtime_error(msg) {}
  };

  class MisAlignedAddrException : public std::runtime_error {
  public:
    MisAlignedAddrException(const char *msg) : std::runtime_error(msg) {}
  };

  enum struct MemoryOp { STORE = 0, LOAD = 1 };

  using listIt = std::list<Page>::iterator;
  using PTLowLvl = std::unordered_map<uint16_t, listIt>;
  using PTHighLvl = std::unordered_map<uint16_t, PTLowLvl>;

  PhysMemory() = default;

  listIt getNewPage(uint16_t pt1, uint16_t pt2);

  template <MemoryOp op> listIt pageTableLookup(const AddrSections &sect);

  template <typename T, PhysMemory::MemoryOp op> T *getEntity(Addr addr);
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
  void storeRange(Addr start, It begin, It end);
};

/*
class TLB
{
  struct TLBEntry
  {
    Addr VirtualAddress{};
    listIt physPage{};
  }

  private:
  std::unordered_map<
};
*/

template <typename T, PhysMemory::MemoryOp op>
inline T *PhysMemory::getEntity(Addr addr) {
  if (addr % sizeof(T) || ((getOffset(addr) + sizeof(T)) > (1 << kOffsetBits)))
    throw PhysMemory::MisAlignedAddrException(
        "Misaligned memory access is not supported!");
  AddrSections sections(addr);
  auto offset = sections.offset;
  auto it = PhysMemory::pageTableLookup<op>(sections);
  Word *word = &it->wordStorage.at(offset / sizeof(Word));
  Byte *byte = reinterpret_cast<Byte *>(word) + (offset % sizeof(Word));
  return reinterpret_cast<T *>(byte);
}

template <PhysMemory::MemoryOp op>
PhysMemory::listIt PhysMemory::pageTableLookup(const AddrSections &sect) {
  using MemOp = PhysMemory::MemoryOp;
  auto it_P1 = pageTable.find(sect.indexPt1);
  if (it_P1 == pageTable.end()) {
    if constexpr (op == MemOp::LOAD)
      throw PhysMemory::PageFaultException(
          "Load on unmapped region in physical mem (P1)");
    else {
      return getNewPage(sect.indexPt1, sect.indexPt2);
    }
  }
  auto &pageTableLowLvl = pageTable.at(sect.indexPt1);

  auto it_P2 = pageTableLowLvl.find(sect.indexPt2);
  if (it_P2 == pageTableLowLvl.end()) {
    if constexpr (op == MemOp::LOAD)
      throw PhysMemory::PageFaultException(
          "Load on unmapped region in physical mem (P2)");
    else {
      return getNewPage(sect.indexPt1, sect.indexPt2);
    }
  }
  return pageTableLowLvl.at(sect.indexPt2);
}

inline PhysMemory::listIt PhysMemory::getNewPage(uint16_t pt1, uint16_t pt2) {
  pageStorage.emplace_back();
  auto new_page = std::prev(pageStorage.end());
  pageTable[pt1][pt2] = new_page;
  return new_page;
}

template <std::forward_iterator It>
void Memory::storeRange(Addr start, It begin, It end) {
  std::for_each(begin, end, [&start, this](auto curWord) {
    storeWord(start, curWord);
    start += kXLENInBytes;
  });
}

inline void Memory::printMemStats(std::ostream &ost) const {
  ost << "Memory stats:" << std::endl;
  ost << "Loads: " << stats.numLoads << std::endl;
  ost << "Stores: " << stats.numStores << std::endl;
  ost << "End of memory stats." << std::endl;
}

inline const Memory::MemoryStats &Memory::getMemStats() const { return stats; }

inline uint16_t PhysMemory::getOffset(Addr addr) {
  return static_cast<uint16_t>(getBits<kOffsetBits - 1, 0>(addr));
}

inline Word Memory::loadWord(Addr addr) {
  stats.numLoads++;
  Word loadedWord = *physMem.getEntity<Word, PhysMemory::MemoryOp::LOAD>(addr);

  return loadedWord;
}

inline void Memory::storeWord(Addr addr, Word word) {
  stats.numStores++;
  *physMem.getEntity<Word, PhysMemory::MemoryOp::STORE>(addr) = word;
}

inline Half Memory::loadHalf(Addr addr) {
  stats.numLoads++;
  Half loadedWord = *physMem.getEntity<Half, PhysMemory::MemoryOp::LOAD>(addr);

  return loadedWord;
}

inline void Memory::storeHalf(Addr addr, Half half) {
  stats.numStores++;
  *physMem.getEntity<Half, PhysMemory::MemoryOp::STORE>(addr) = half;
}

inline Byte Memory::loadByte(Addr addr) {
  stats.numLoads++;
  Byte loadedWord = *physMem.getEntity<Byte, PhysMemory::MemoryOp::LOAD>(addr);

  return loadedWord;
}

inline void Memory::storeByte(Addr addr, Byte byte) {
  stats.numStores++;
  *physMem.getEntity<Byte, PhysMemory::MemoryOp::STORE>(addr) = byte;
}

} // namespace sim

#endif // __INCLUDE_MEMORY_MEMORY_HH__
