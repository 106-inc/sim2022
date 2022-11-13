#include "test_header.hh"

#include "common/common.hh"
#include "memory/memory.hh"

constexpr std::size_t kNumReqs = 10;

using sim::Addr;

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

#include "test_footer.hh"
