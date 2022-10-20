#include <gtest/gtest.h>

#include "common/common.hh"
#include "memory/memory.hh"

constexpr std::size_t kNumReqs = 10;

TEST(Memory, Memory_store) {

    sim::Memory mem;
    sim::Memory::MemoryStats stats;
    for (std::size_t i = 0; i < kNumReqs; ++i)
       mem.storeWord(i, i * i);

    stats = mem.getMemStats();
    EXPECT_EQ(mem.getCurrMemSize(), kNumReqs);
    EXPECT_EQ(stats.numStores, kNumReqs);
    EXPECT_EQ(stats.numLoads, 0);
    EXPECT_EQ(stats.numPageFaults, 0);
}

TEST(Memory, Memory_load) {

    sim::Memory mem;
    sim::Memory::MemoryStats stats;
    for (std::size_t i = 0; i < kNumReqs; ++i)
      mem.storeWord(i, i * i);

    for (std::size_t i = 0; i < kNumReqs; ++i)
      EXPECT_EQ(mem.loadWord(i), i * i);

    stats = mem.getMemStats();
    EXPECT_EQ(mem.getCurrMemSize(), kNumReqs);
    EXPECT_EQ(stats.numStores, kNumReqs);
    EXPECT_EQ(stats.numLoads, kNumReqs);
    EXPECT_EQ(stats.numPageFaults, 0);
}

TEST(Memory, Memory_page_fault) {

    sim::Memory mem;
    sim::Memory::MemoryStats stats;
    for (std::size_t i = 0; i < kNumReqs; ++i)
      mem.storeWord(i, i * i);

    EXPECT_EQ(mem.loadWord(kNumReqs + 1), sim::kDummyWord);

    stats = mem.getMemStats();
    EXPECT_EQ(mem.getCurrMemSize(), 10);
    EXPECT_EQ(stats.numStores, 10);
    EXPECT_EQ(stats.numLoads, 1);
    EXPECT_EQ(stats.numPageFaults, 1);
}