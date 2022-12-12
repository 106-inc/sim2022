#include "memory/memory.hh"

namespace sim {

void Memory::printMemStats(std::ostream &ost) const {
  ost << "Memory stats:" << std::endl;
  ost << "Loads: " << stats.numLoads << std::endl;
  ost << "Stores: " << stats.numStores << std::endl;
  ost << "End of memory stats." << std::endl;
}

const Memory::MemoryStats &Memory::getMemStats() const { return stats; }

PhysMemory::listIt PhysMemory::pageTableLookup(const AddrSections &sect,
                                               MemoryOp op) {
  using MemOp = PhysMemory::MemoryOp;
  auto it_P1 = pageTable.find(sect.index_pt1);
  if (it_P1 == pageTable.end()) {
    if (op == MemOp::LOAD)
      throw std::runtime_error("Load on unmapped region in physical mem (P1)");
    else {
      pageStorage.emplace_back(Page{});
      pageTable[sect.index_pt1][sect.index_pt2] = std::prev(pageStorage.end());
    }
  }
  auto &pageTableLowLvl = pageTable.at(sect.index_pt1);

  auto it_P2 = pageTableLowLvl.find(sect.index_pt2);
  if (it_P2 == pageTableLowLvl.end()) {
    if (op == MemOp::LOAD)
      throw std::runtime_error("Load on unmapped region in physical mem (P2)");
    else {
      pageStorage.emplace_back(Page{});
      pageTableLowLvl[sect.index_pt2] = std::prev(pageStorage.end());
    }
  }
  return pageTableLowLvl.at(sect.index_pt2);
}

uint16_t PhysMemory::getOffset(Addr addr) {
  return static_cast<uint16_t>(getBits<kOffsetBits - 1, 0>(addr));
}

} // namespace sim
