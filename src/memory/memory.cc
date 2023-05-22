#include "memory/memory.hh"

namespace sim {

//~~~~~Memory class functions~~~~~

void Memory::printMemStats(std::ostream &ost) const {
  ost << "Memory stats:" << std::endl;
  ost << "Loads: " << stats.numLoads << std::endl;
  ost << "Stores: " << stats.numStores << std::endl;
  ost << "End of memory stats." << std::endl;
}

const Memory::MemoryStats &Memory::getMemStats() const { return stats; }

uint16_t PhysMemory::getOffset(Addr addr) {
  return static_cast<uint16_t>(getBits<kOffsetBits - 1, 0>(addr));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~TLB class functions~~~~~~~~
TLB::TLBIndex TLB::getTLBIndex(Addr addr) {
  return static_cast<TLB::TLBIndex>(
      getBits<(kTLBBits + kOffsetBits - 1), kOffsetBits>(addr));
}

PagePtr TLB::tlbLookup(Addr addr) {
  stats.TLBRequests++;
  auto idx = getTLBIndex(addr);
  auto found = tlb[idx];
  if (!found.valid) {
    stats.TLBMisses++;
    return nullptr;
  }
  if (found.virtualAddress != (addr & kTLBMask)) {
    stats.TLBMisses++;
    return nullptr;
  }
  stats.TLBHits++;
  return found.physPage;
}

void TLB::tlbUpdate(Addr addr, PagePtr page) {

  auto idx = getTLBIndex(addr);
  tlb[idx] = TLBEntry((addr & kTLBMask), page, true);
}

const TLB::TLBStats &TLB::getTLBStats() const { return stats; }

void TLB::tlbFlush() { tlb.clear(); }

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

} // namespace sim
