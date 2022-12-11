#include "test_header.hh"

#include "common/common.hh"
#include "memory/memory.hh"

constexpr std::size_t kNumReqs = 10;

using sim::Addr;
using AddrSections = sim::PhysMemory::AddrSections;
using Page = sim::PhysMemory::Page;
using MemOp = sim::PhysMemory::MemoryOp;

TEST(Memory, Memory_store_load) {

  sim::Memory mem;
  sim::Memory::MemoryStats stats;

  // Load value which was stored  previously
  for (Addr i = 0; i < kNumReqs; ++i)
    mem.storeWord(i * 4, i * i);
  for (Addr i = 0; i < kNumReqs; ++i)
    EXPECT_EQ(mem.loadWord(i * 4), i * i);
}

TEST(Memory, Mem_stats) {

  sim::Memory mem;
  sim::Memory::MemoryStats stats;

  // kNumReqs stores
  // 2 * kNumReqs loads (kNumReqs pageFaults + kNumReqs real loads)
  for (Addr i = 0; i < kNumReqs; ++i)
    mem.storeWord(i * 4, i * i);
  for (Addr i = 0; i < kNumReqs; ++i)
    mem.loadWord(i * 4);
  for (Addr i = kNumReqs; i < 2 * kNumReqs; ++i)
    mem.loadWord(i * 4);

  stats = mem.getMemStats();
  EXPECT_EQ(stats.numStores, kNumReqs);
  EXPECT_EQ(stats.numLoads, 2 * kNumReqs);
}

TEST(PhysMemory, getVirtAddrSections) {

  sim::PhysMemory phMem;

  EXPECT_EQ(phMem.getVirtAddrSections(0xDEADBEEF), AddrSections(890, 731, 3823));
  EXPECT_EQ(phMem.getVirtAddrSections(0x0), AddrSections(0, 0, 0));
  EXPECT_EQ(phMem.getVirtAddrSections(0xFFFFFFFF), AddrSections(1023, 1023, 4095));
}


TEST(PhysMemory, pageTableLookup) {

  sim::PhysMemory phMem;
  //Load on unmapped region (P1) and than store to it
  AddrSections page1(0, 0, 0);
  EXPECT_ANY_THROW(phMem.pageTableLookup(page1, MemOp::LOAD));
  auto iter_1 = phMem.pageTableLookup(page1, MemOp::STORE);

  //Load on unmapped region (P2) and than store to it
  AddrSections page2(0, 1, 0);
  EXPECT_ANY_THROW(phMem.pageTableLookup(page2, MemOp::LOAD));
  auto iter_2 = phMem.pageTableLookup(page2, MemOp::STORE);

  //Check previously stored values
  EXPECT_EQ(phMem.pageTableLookup(page1, MemOp::LOAD), iter_1);
  EXPECT_EQ(phMem.pageTableLookup(page2, MemOp::LOAD), iter_2);

  //Check that stores point on the same pages
  AddrSections page3(3, 2, 1);
  auto iter_3 = phMem.pageTableLookup(page3, MemOp::STORE);
  auto iter_4 = phMem.pageTableLookup(page3, MemOp::STORE);
  EXPECT_EQ(iter_3, iter_4);
  EXPECT_EQ(phMem.pageTableLookup(page3, MemOp::LOAD), iter_3);

}

TEST(PhysMemory, getWord) {
  sim::PhysMemory phMem;
  //Misaligned Address
  EXPECT_ANY_THROW(phMem.getWord(0xDEADBEEF, MemOp::LOAD));

  //Load on unmapped region
  EXPECT_ANY_THROW(phMem.getWord(0x10000000, MemOp::LOAD));

  phMem.getWord(0x10000000, MemOp::STORE) = 42;
  EXPECT_EQ(phMem.getWord(0x10000000, MemOp::LOAD), 42);

  //Rewriting word on 0x10000000
  phMem.getWord(0x10000000, MemOp::STORE) = 0;
  phMem.getWord(0x10000010, MemOp::STORE) = 1;
  phMem.getWord(0x10000020, MemOp::STORE) = 2;
  phMem.getWord(0x10000030, MemOp::STORE) = 3;
  EXPECT_EQ(phMem.getWord(0x10000000, MemOp::LOAD), 0);
  EXPECT_EQ(phMem.getWord(0x10000010, MemOp::LOAD), 1);
  EXPECT_EQ(phMem.getWord(0x10000020, MemOp::LOAD), 2);
  EXPECT_EQ(phMem.getWord(0x10000030, MemOp::LOAD), 3);

}

#include "test_footer.hh"
