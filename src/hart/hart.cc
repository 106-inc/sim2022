#include <memory>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <utility>

#include "common/common.hh"
#include "common/inst.hh"
#include "elfloader/elfloader.hh"
#include "hart/hart.hh"

namespace sim {

class InfCache final : public IBBCache {
  std::unordered_map<Addr, BasicBlock> hash_{};

public:
  const BasicBlock &
  lookupUpdate(Addr key, std::function<BasicBlock(Addr)> slowGetData) override {
    if (auto hit = hash_.find(key); hit != hash_.end())
      return hit->second;

    auto bb = slowGetData(key);
    return hash_[key] = std::move(bb);
  }
};

class NoCache final : public IBBCache {
public:
  const BasicBlock &
  lookupUpdate(Addr key, std::function<BasicBlock(Addr)> slowGetData) override {
    static BasicBlock cur{};
    cur = slowGetData(key);
    return cur;
  }
};

class LRUCache final : public IBBCache {
  std::size_t size_;
  std::list<std::pair<Addr, BasicBlock>> cache_{};

  using ListIt = typename std::list<std::pair<Addr, BasicBlock>>::iterator;
  std::unordered_map<Addr, ListIt> hash_{};

public:
  explicit LRUCache(std::size_t size) : size_(size) {}

  [[nodiscard]] bool isFull() const { return (cache_.size() == size_); }

  const BasicBlock &
  lookupUpdate(Addr key, std::function<BasicBlock(Addr)> slowGetData) override {
    auto hit = hash_.find(key);
    if (hit == hash_.end()) {
      if (isFull()) {
        hash_.erase(cache_.back().first);
        cache_.pop_back();
      }
      cache_.emplace_front(key, slowGetData(key));
      hash_.emplace(key, cache_.begin());
    } else if (auto eltit = hit->second; eltit != cache_.begin())
      cache_.splice(cache_.begin(), cache_, eltit, std::next(eltit));

    return cache_.front().second;
  }
};

Hart::Hart(const fs::path &executable, std::int64_t bbCacheSize) {
  if (bbCacheSize < 0)
    bbc_ = std::make_unique<InfCache>();
  else if (bbCacheSize == 0)
    bbc_ = std::make_unique<NoCache>();
  else
    bbc_ = std::make_unique<LRUCache>(bbCacheSize);

  ELFLoader loader{executable};
  getPC() = loader.getEntryPoint();

  for (auto segmentIdx : loader.getLoadableSegments()) {
    auto text = loader.getSegment(segmentIdx);
    auto addr = loader.getSegmentAddr(segmentIdx);
    getMem().storeRange(addr, text.begin(), text.end());

    auto fileSize = static_cast<Addr>(loader.getSegmentFileSize(segmentIdx));
    auto memSize = static_cast<Addr>(loader.getSegmentMemorySize(segmentIdx));
    for (; fileSize < memSize; fileSize += kXLENInBytes)
      getMem().storeWord(addr + fileSize, Word{});
  }

  getMem().setProgramStoredFlag();
}

BasicBlock Hart::createBB(Addr addr) {
  BasicBlock bb{};

#ifdef SPDLOG
  spdlog::trace("Creating basic block:");
#endif
  for (bool isBranch = false; !isBranch; addr += kXLENInBytes) {
    auto inst = decoder_.decode(getMem().loadWord(addr));
    if (inst.type == OpType::UNKNOWN)
      throw std::logic_error{
          "Unknown instruction found while decoding basic block" + inst.str()};

#ifdef SPDLOG
    spdlog::trace(inst.str());
#endif
    isBranch = inst.isBranch;
    bb.push_back(inst);
  }

#ifdef SPDLOG
  spdlog::trace("Basic blok created.");
#endif
  return bb;
}

void Hart::run() {
  auto lCreateBB = [this](Addr addr) { return createBB(addr); };

  while (!state_.complete) {
    const auto &bb = bbc_->lookupUpdate(getPC(), lCreateBB);
    exec_.execute(bb.begin(), bb.end(), state_);
  }
  auto stats = state_.mem.getTLBStats();
  std::cout << "Hits: "
            << static_cast<double>(stats.TLBHits) /
                   static_cast<double>(stats.TLBRequests) * 100.0
            << std::endl;
}

} // namespace sim
