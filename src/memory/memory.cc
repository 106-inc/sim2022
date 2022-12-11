#include <algorithm>

#include "memory/memory.hh"

namespace sim {

Word Memory::loadWord(Addr addr) {
  stats.numLoads++;
  Word word = kDummyWord;

  auto it = mem.find(addr);
  if (it != mem.end())
    word = it->second;
  else
    word = pageFaultHandle(addr);

  return word;
}

void Memory::storeWord(Addr addr, Word word) {
  stats.numStores++;
  mem[addr] = word;
}

Word Memory::pageFaultHandle(Addr addr) {
  (void)addr;
  stats.numPageFaults++;
  std::cout << "Placeholder for page fault" << std::endl;
  return kDummyWord;
}

std::size_t Memory::getCurrMemSize() const { return mem.size(); }

void Memory::printMemStats(std::ostream &ost) const {
  ost << "Memory stats:" << std::endl;
  ost << "Loads: " << stats.numLoads << std::endl;
  ost << "Stores: " << stats.numStores << std::endl;
  ost << "PageFaults: " << stats.numPageFaults << std::endl;
  ost << "End of memory stats." << std::endl;
}

const Memory::MemoryStats &Memory::getMemStats() const { return stats; }

PhysMemory::AddrSections PhysMemory::getVirtAddrSections(Addr addr) {
  constexpr uint16_t offsetShift = 12;
  constexpr uint16_t offsetMask = (1u << 12) - 1;
  constexpr uint16_t index_P1Mask = (1u << 10) - 1;
  constexpr uint16_t index_P2Mask = (1u << 10) - 1;
  constexpr uint16_t index_P1Shift = 22;

  uint16_t index_p1 = (addr >> index_P1Shift) & index_P1Mask;
  uint16_t index_p2 = (addr >> offsetShift) & index_P2Mask;
  uint16_t offset = addr & offsetMask;
  return PhysMemory::AddrSections(index_p1, index_p2, offset);
}

PhysMemory::listIt PhysMemory::pageTableLookup(const AddrSections &sect,
                                               MemoryOp op) {
  using MemOp = PhysMemory::MemoryOp;
  auto it_P1 = pageTable.find(sect.index_pt1);
  if (it_P1 == pageTable.end()) {
    if (op == MemOp::LOAD)
      throw std::runtime_error("Load on unmapped region in physical mem (P1)");
    else {
      pageStorage.push_back(Page{});
      pageTable[sect.index_pt1][sect.index_pt2] = std::prev(pageStorage.end());
    }
  }
  auto &pageTableLowLvl = pageTable.at(sect.index_pt1);

  auto it_P2 = pageTableLowLvl.find(sect.index_pt2);
  if (it_P2 == pageTableLowLvl.end()) {
    if (op == MemOp::LOAD)
      throw std::runtime_error("Load on unmapped region in physical mem (P2)");
    else {
      pageStorage.push_back(Page{});
      pageTableLowLvl[sect.index_pt2] = std::prev(pageStorage.end());
    }
  }
  return pageTableLowLvl.at(sect.index_pt2);
}

} // namespace sim
