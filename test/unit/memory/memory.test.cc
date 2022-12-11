#include "test_header.hh"

#include "common/common.hh"
#include "memory/memory.hh"

constexpr std::size_t kNumReqs = 10;

using sim::Addr;
using AddrSections = sim::PhysMemory::AddrSections;
using Page = sim::PhysMemory::Page;

TEST(Memory, Memory_store_load) {

  sim::Memory mem;
  sim::Memory::MemoryStats stats;

  // Load value which was stored  previously
  for (Addr i = 0; i < kNumReqs; ++i)
    mem.storeWord(i, i * i);
  for (Addr i = 0; i < kNumReqs; ++i)
    EXPECT_EQ(mem.loadWord(i), i * i);
  // Load value which was stored  previously (page fault)
  for (Addr i = kNumReqs; i < 2 * kNumReqs; ++i)
    EXPECT_EQ(mem.loadWord(i), sim::kDummyWord);
}

TEST(Memory, Mem_stats) {

  sim::Memory mem;
  sim::Memory::MemoryStats stats;

  // kNumReqs stores
  // 2 * kNumReqs loads (kNumReqs pageFaults + kNumReqs real loads)
  for (Addr i = 0; i < kNumReqs; ++i)
    mem.storeWord(i, i * i);
  for (Addr i = 0; i < kNumReqs; ++i)
    mem.loadWord(i);
  for (Addr i = kNumReqs; i < 2 * kNumReqs; ++i)
    mem.loadWord(i);

  stats = mem.getMemStats();
  EXPECT_EQ(mem.getCurrMemSize(), kNumReqs);
  EXPECT_EQ(stats.numStores, kNumReqs);
  EXPECT_EQ(stats.numLoads, 2 * kNumReqs);
  EXPECT_EQ(stats.numPageFaults, kNumReqs);
}

TEST(PhysMemory, getVirtAddrSections) {

  sim::PhysMemory phMem;

  EXPECT_EQ(phMem.getVirtAddrSections(0xDEADBEEF), AddrSections(890, 731, 3823));
  EXPECT_EQ(phMem.getVirtAddrSections(0x0), AddrSections(0, 0, 0));
  EXPECT_EQ(phMem.getVirtAddrSections(0xFFFFFFFF), AddrSections(1023, 1023, 4095));
}

#include "test_footer.hh"
