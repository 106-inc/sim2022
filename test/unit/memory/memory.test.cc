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

TEST(PhysMemory, AddrSectionsCtor) {

  sim::PhysMemory phMem;

  EXPECT_EQ(AddrSections(0xDEADBEEF), AddrSections(890, 731, 3823));
  EXPECT_EQ(AddrSections(0x0), AddrSections(0, 0, 0));
  EXPECT_EQ(AddrSections(0xFFFFFFFF), AddrSections(1023, 1023, 4095));
}


TEST(PhysMemory, pageTableLookup) {

  sim::PhysMemory phMem;
  //Load on unmapped region (P1) and than store to it
  AddrSections page1(0, 0, 0);
  EXPECT_ANY_THROW(phMem.pageTableLookup<MemOp::LOAD>(page1));
  auto iter_1 = phMem.pageTableLookup<MemOp::STORE>(page1);

  //Load on unmapped region (P2) and than store to it
  AddrSections page2(0, 1, 0);
  EXPECT_ANY_THROW(phMem.pageTableLookup<MemOp::LOAD>(page2));
  auto iter_2 = phMem.pageTableLookup<MemOp::STORE>(page2);

  //Check previously stored values
  EXPECT_EQ(phMem.pageTableLookup<MemOp::LOAD>(page1), iter_1);
  EXPECT_EQ(phMem.pageTableLookup<MemOp::LOAD>(page2), iter_2);

  //Check that stores point on the same pages
  AddrSections page3(3, 2, 1);
  auto iter_3 = phMem.pageTableLookup<MemOp::STORE>(page3);
  auto iter_4 = phMem.pageTableLookup<MemOp::STORE>(page3);
  EXPECT_EQ(iter_3, iter_4);
  EXPECT_EQ(phMem.pageTableLookup<MemOp::LOAD>(page3), iter_3);

}

TEST(PhysMemory, MixingWordHalf)
{
  sim::Memory mem;
  mem.storeWord(0x10000000, 0xAABBCCDD);
  EXPECT_EQ(mem.loadWord(0x10000000), 0xAABBCCDD);
  EXPECT_EQ(mem.loadHalf(0x10000000), 0xCCDD);
  EXPECT_EQ(mem.loadHalf(0x10000002), 0xAABB);
  EXPECT_EQ(mem.loadByte(0x10000000), 0xDD);
  EXPECT_EQ(mem.loadByte(0x10000001), 0xCC);
  EXPECT_EQ(mem.loadByte(0x10000002), 0xBB);
  EXPECT_EQ(mem.loadByte(0x10000003), 0xAA);

}

TEST(PhysMemory, getOffset)
{
  sim::PhysMemory phMem;
  EXPECT_EQ(phMem.getOffset(0xDEADBEEF),  3823);
  EXPECT_EQ(phMem.getOffset(0x0),  0);
  EXPECT_EQ(phMem.getOffset(0xFFFFFFFF),  4095);

}

TEST(PhysMemory, getEntity) {
  sim::Memory mem;
  //Misaligned Address (Not alogned to word_size)
  EXPECT_ANY_THROW(mem.storeWord(0xDEADBE01, 0x0));
  //Misaligned Address (Change the page)
  EXPECT_ANY_THROW(mem.storeWord(0xDEADFFFE, 0x0));
  //Load on unmapped region
  EXPECT_ANY_THROW(mem.loadWord(0x0));

  mem.storeWord(0x10000000, 42);
  EXPECT_EQ(mem.loadWord(0x10000000), 42);

  //Rewriting word
  mem.storeWord(0x10000000, 21);
  EXPECT_EQ(mem.loadWord(0x10000000), 21);
}

#include "test_footer.hh"
