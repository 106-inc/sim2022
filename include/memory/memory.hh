#ifndef __INCLUDE_MEMORY_MEMORY_HH__
#define __INCLUDE_MEMORY_MEMORY_HH__

#include "common/common.hh"
#include <algorithm>
#include <concepts>
#include <iostream>
#include <list>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

#include "common/common.hh"

namespace sim {

struct Page {
  Page() { wordStorage.resize(kPageSize / sizeof(Word)); }

  std::vector<Word> wordStorage{};
};

using PT = std::unordered_map<uint32_t, Page>;
using PagePtr = Page *;

/*
  We use pointers on std::unordered_map::value because
  (§23.2.5/13) The insert and emplace members shall not affect the validity
  of references to container elements, but may invalidate all iterators
  to the container.
  The erase members shall invalidate only iterators and references
  to the erased elements.
*/

class TLB final {
public:
  using TLBIndex = uint16_t;
  struct TLBEntry {
    Addr virtualAddress{0};
    PagePtr physPage{nullptr};
    bool valid{false};
    TLBEntry() = default;
    TLBEntry(Addr addr, PagePtr page, bool vld)
        : virtualAddress(addr), physPage(page), valid(vld) {}
  };

  struct TLBStats {
    std::size_t TLBHits{};
    std::size_t TLBMisses{};
    std::size_t TLBRequests{};

    TLBStats() = default;
  };

  TLB() { tlb.resize(kTLBSize); }
  PagePtr tlbLookup(Addr addr);
  void tlbUpdate(Addr addr, PagePtr page);
  TLBIndex getTLBIndex(Addr addr);
  const TLBStats &getTLBStats() const;
  void tlbFlush();

private:
  std::vector<TLBEntry> tlb{};
  TLBStats stats{};
};

template <typename T>
concept isSimType =
    std::same_as<T, Word> || std::same_as<T, Byte> || std::same_as<T, Half>;

class PhysMemory final {
public:
  struct AddrSections {
    uint32_t indexPt{};
    uint16_t offset{};

    AddrSections(uint32_t pt, uint16_t off) : indexPt(pt), offset(off) {}
    AddrSections(Addr addr) {
      constexpr std::pair<uint8_t, uint8_t> of_bits{kOffsetBits - 1, 0};
      constexpr std::pair<uint8_t, uint8_t> pt_bits{sizeofBits<Addr>() - 1,
                                                    kOffsetBits};
      indexPt = getBits<pt_bits.first, pt_bits.second>(addr);
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

  PhysMemory() = default;

  template <MemoryOp op> PagePtr pageTableLookup(const AddrSections &sect);

  template <isSimType T, PhysMemory::MemoryOp op> T *getEntity(Addr addr);
  uint16_t getOffset(Addr addr);

  const TLB::TLBStats getTLBStats() { return tlb.getTLBStats(); }

private:
  PT pageTable{};
  TLB tlb{};
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
  bool isProgramStored{false};

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

  void setProgramStoredFlag() { isProgramStored = true; }

  void printMemStats(std::ostream &ost) const;
  const MemoryStats &getMemStats() const;

  template <std::forward_iterator It>
  void storeRange(Addr start, It begin, It end);

  const TLB::TLBStats getTLBStats() { return physMem.getTLBStats(); }
};

template <isSimType T, PhysMemory::MemoryOp op>
inline T *PhysMemory::getEntity(Addr addr) {
#ifdef MISALIGNED_CHECK
  if (addr % sizeof(T) || ((getOffset(addr) + sizeof(T)) > (1 << kOffsetBits)))
    throw PhysMemory::MisAlignedAddrException(
        "Misaligned memory access is not supported!");
#endif
  AddrSections sections(addr);
  auto offset = sections.offset;
  auto isInTLB = tlb.tlbLookup(addr);
  PagePtr page;
  if (isInTLB) {
    page = isInTLB;
  } else {
    page = PhysMemory::pageTableLookup<op>(sections);
    tlb.tlbUpdate(addr, page);
  }
  Word *word = &page->wordStorage.at(offset / sizeof(Word));
  Byte *byte = reinterpret_cast<Byte *>(word) + (offset % sizeof(Word));
  return reinterpret_cast<T *>(byte);
}

template <PhysMemory::MemoryOp op>
PagePtr PhysMemory::pageTableLookup(const AddrSections &sect) {

  using MemOp = PhysMemory::MemoryOp;
  auto index = sect.indexPt;
  auto it_PT = pageTable.find(index);
  if (it_PT == pageTable.end()) {
    if constexpr (op == MemOp::LOAD)
      throw PhysMemory::PageFaultException(
          "Load on unmapped region in physical mem");
    else {
      pageTable[index] = Page();
    }
  }
  return &pageTable.at(index);
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
#ifdef SPDLOG
  if (isProgramStored) {
    cosimLog("M[0x{:08x}]=0x{:08x}", addr, word);
  }
#endif
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

inline TLB::TLBIndex TLB::getTLBIndex(Addr addr) {
  return static_cast<TLB::TLBIndex>(
      getBits<(kTLBBits + kOffsetBits - 1), kOffsetBits>(addr));
}

inline PagePtr TLB::tlbLookup(Addr addr) {
  stats.TLBRequests++;
  auto idx = getTLBIndex(addr);
  auto found = tlb[idx];
  if (!found.valid) {
    stats.TLBMisses++;
    return nullptr;
  }
  if (found.virtualAddress != addr) {
    stats.TLBMisses++;
    return nullptr;
  }
  stats.TLBHits++;
  return found.physPage;
}

inline void TLB::tlbUpdate(Addr addr, PagePtr page) {

  auto idx = getTLBIndex(addr);
  tlb[idx] = TLBEntry(addr, page, true);
}

inline const TLB::TLBStats &TLB::getTLBStats() const { return stats; }

inline void TLB::tlbFlush() { tlb.clear(); }

} // namespace sim

// dummy comment
#endif // __INCLUDE_MEMORY_MEMORY_HH__
