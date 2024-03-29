#include "test_header.hh"

#include "common/common.hh"
#include "memory/memory.hh"

constexpr std::size_t kNumReqs = 10;

using sim::Addr;
using AddrSections = sim::PhysMemory::AddrSections;
using Page = sim::Page;
using PagePtr = sim::PagePtr;
using MemOp = sim::PhysMemory::MemoryOp;
using Word = sim::Word;
using Byte = sim::Byte;
using Half = sim::Half;

TEST(Memory, Memory_store_load) {
  sim::Memory mem;

  // Load value which was stored  previously
  for (Addr i = 0; i < kNumReqs; ++i)
    mem.storeEntity<Word>(i * 4, i * i);
  for (Addr i = 0; i < kNumReqs; ++i)
    EXPECT_EQ(mem.loadEntity<Word>(i * 4), i * i);
}

TEST(Memory, Mem_stats) {
  sim::Memory mem;
  sim::Memory::MemoryStats stats;

  // kNumReqs stores
  // 2 * kNumReqs loads (kNumReqs pageFaults + kNumReqs real loads)
  for (Addr i = 0; i < kNumReqs; ++i)
    mem.storeEntity<Word>(i * 4, i * i);
  for (Addr i = 0; i < kNumReqs; ++i)
    mem.loadEntity<Word>(i * 4);
  for (Addr i = kNumReqs; i < 2 * kNumReqs; ++i)
    mem.loadEntity<Word>(i * 4);

  stats = mem.getMemStats();
  EXPECT_EQ(stats.numStores, kNumReqs);
  EXPECT_EQ(stats.numLoads, 2 * kNumReqs);
}

TEST(PhysMemory, AddrSectionsCtor) {

  sim::PhysMemory phMem;
  // 0xDEADB, 0xEEF
  EXPECT_EQ(AddrSections(0xDEADBEEF), AddrSections(912091, 3823));
  EXPECT_EQ(AddrSections(0x0), AddrSections(0, 0));
  // 2^20 - 1, 2^12 - 1
  EXPECT_EQ(AddrSections(0xFFFFFFFF), AddrSections(1048575, 4095));
}

TEST(PhysMemory, pageTableLookup) {

  sim::PhysMemory phMem;
  // Load on unmapped region (P1) and than store to it
  AddrSections page1(0, 0);
  EXPECT_THROW(phMem.pageTableLookup<MemOp::LOAD>(page1),
               sim::PhysMemory::PageFaultException);
  PagePtr ptr_1 = phMem.pageTableLookup<MemOp::STORE>(page1);

  // Load on unmapped region (P2) and than store to it
  AddrSections page2(1, 1);
  EXPECT_THROW(phMem.pageTableLookup<MemOp::LOAD>(page2),
               sim::PhysMemory::PageFaultException);
  PagePtr ptr_2 = phMem.pageTableLookup<MemOp::STORE>(page2);

  // Check previously stored values
  EXPECT_EQ(phMem.pageTableLookup<MemOp::LOAD>(page1), ptr_1);
  EXPECT_EQ(phMem.pageTableLookup<MemOp::LOAD>(page2), ptr_2);

  // Check that stores point on the same pages
  AddrSections page3(3, 2);
  auto ptr_3 = phMem.pageTableLookup<MemOp::STORE>(page3);
  auto ptr_4 = phMem.pageTableLookup<MemOp::STORE>(page3);
  EXPECT_EQ(ptr_3, ptr_4);
  EXPECT_EQ(phMem.pageTableLookup<MemOp::LOAD>(page3), ptr_3);
}

TEST(PhysMemory, MixingWordHalf) {
  sim::Memory mem;
  mem.storeEntity<Word>(0x10000000, 0xAABBCCDD);
  EXPECT_EQ(mem.loadEntity<Word>(0x10000000), 0xAABBCCDD);
  EXPECT_EQ(mem.loadEntity<Half>(0x10000000), 0xCCDD);
  EXPECT_EQ(mem.loadEntity<Half>(0x10000002), 0xAABB);
  EXPECT_EQ(mem.loadEntity<Byte>(0x10000000), 0xDD);
  EXPECT_EQ(mem.loadEntity<Byte>(0x10000001), 0xCC);
  EXPECT_EQ(mem.loadEntity<Byte>(0x10000002), 0xBB);
  EXPECT_EQ(mem.loadEntity<Byte>(0x10000003), 0xAA);
}

TEST(PhysMemory, getOffset) {
  sim::PhysMemory phMem;
  EXPECT_EQ(phMem.getOffset(0xDEADBEEF), 3823);
  EXPECT_EQ(phMem.getOffset(0x0), 0);
  EXPECT_EQ(phMem.getOffset(0xFFFFFFFF), 4095);
}

TEST(PhysMemory, getEntity) {
  sim::Memory mem;
#ifdef MISALIGNED_CHECK
  // Misaligned Address (Not alogned to word_size)
  EXPECT_THROW(mem.storeEntity<Word>(0xDEADBE01, 0x0),
               sim::PhysMemory::MisAlignedAddrException);
  // Misaligned Address (Between the pages)
  EXPECT_THROW(mem.storeEntity<Word>(0xDEADFFFE, 0x0),
               sim::PhysMemory::MisAlignedAddrException);
#endif
  // Load on unmapped region
  EXPECT_THROW(mem.loadEntity<Word>(0x0), sim::PhysMemory::PageFaultException);

  mem.storeEntity<Word>(0x10000000, 42);
  EXPECT_EQ(mem.loadEntity<Word>(0x10000000), 42);

  // Rewriting word
  mem.storeEntity<Word>(0x10000000, 21);
  EXPECT_EQ(mem.loadEntity<Word>(0x10000000), 21);
}

TEST(TLB, getTLBIndex) {
  sim::TLB tlb;
  EXPECT_EQ(tlb.getTLBIndex(0xDEADBEEF), 731);
  EXPECT_EQ(tlb.getTLBIndex(0x0), 0);
  EXPECT_EQ(tlb.getTLBIndex(0xFFFFFFFF), 1023);
}

TEST(TLB, tlbLookup) {
  sim::TLB tlb;
  Page page_1{};
  tlb.tlbUpdate(0xDEADBEEF, &page_1);

  // Deadb*** is in TLB, Other patterns - not;
  EXPECT_EQ(tlb.tlbLookup(0xBEEFDEAD), nullptr); // miss
  EXPECT_EQ(tlb.tlbLookup(0xDEADBEEF), &page_1); // hit
  EXPECT_EQ(tlb.tlbLookup(0xDEADBEE0), &page_1); // hit
  EXPECT_EQ(tlb.tlbLookup(0xDEADBE00), &page_1); // hit
  EXPECT_EQ(tlb.tlbLookup(0xDEADB000), &page_1); // hit
  EXPECT_EQ(tlb.tlbLookup(0xDEAD0000), nullptr); // miss

  // Change value in the TLB
  Page page_2{};
  tlb.tlbUpdate(0xDEADBEEF, &page_2);
  EXPECT_EQ(tlb.tlbLookup(0xDEADBEEF), &page_2); // hit
  EXPECT_NE(tlb.tlbLookup(0xDEADBEEF), &page_1); // hit

  // FAADBDEA has the same TLBIndex with DEADBEEF ->
  Page page_3{};
  EXPECT_EQ(tlb.getTLBIndex(0xFAADBDEA), tlb.getTLBIndex(0xDEADBEEF));
  tlb.tlbUpdate(0xFAADBDEA, &page_3);
  EXPECT_EQ(tlb.tlbLookup(0xDEADBEEF), nullptr); // miss
  EXPECT_EQ(tlb.tlbLookup(0xFAADBDEA), &page_3); // hit

  auto stats = tlb.getTLBStats();
  EXPECT_EQ(stats.TLBHits, 7);
  EXPECT_EQ(stats.TLBMisses, 3);
  EXPECT_EQ(stats.TLBRequests, 10);
}

#include "test_footer.hh"
