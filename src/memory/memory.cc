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

} // namespace sim
