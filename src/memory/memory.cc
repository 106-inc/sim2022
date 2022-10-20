#include "memory/memory.hh"

namespace sim {

Memory::Memory() {}

Word Memory::loadWord(AddrType addr) {

  stats.numLoads++;
  Word word = kDummyWord;
  try {
    word = mem.at(addr);
  } catch (const std::out_of_range &) {
    word = pageFaultHandle(addr);
  }
  return word;
}

void Memory::storeWord(AddrType addr, Word word) {

  stats.numStores++;
  mem[addr] = word;
}

Word Memory::pageFaultHandle(AddrType addr) {

  stats.numPageFaults++;
  std::cout << "Placeholder for page fault" << std::endl;
  return kDummyWord;
}

std::size_t Memory::getCurrMemSize() const { return mem.size(); }

void Memory::printMemStats() const {

  std::cout << "Memory stats:" << std::endl;
  std::cout << "Loads: " << stats.numLoads << std::endl;
  std::cout << "Stores: " << stats.numStores << std::endl;
  std::cout << "PageFaults: " << stats.numPageFaults << std::endl;
  std::cout << "End of memory stats." << std::endl;
}

const Memory::MemoryStats &Memory::getMemStats() const { return stats; }

} // namespace sim