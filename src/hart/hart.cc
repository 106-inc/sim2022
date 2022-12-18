#include <spdlog/spdlog.h>

#include "common/common.hh"
#include "common/inst.hh"
#include "elfloader/elfloader.hh"
#include "hart/hart.hh"

namespace sim {

Hart::Hart(const fs::path &executable) {
  ELFLoader loader{executable};
  getPC() = loader.getEntryPoint();

  for (auto segmentIdx : loader.getLoadableSegments()) {
    auto text = loader.getSegment(segmentIdx);
    getMem().storeRange(loader.getSegmentAddr(segmentIdx), text.begin(),
                        text.end());
  }
}

BasicBlock Hart::createBB(Addr addr) {
  BasicBlock bb{};

  spdlog::trace("Creating basic block:");
  for (bool isBranch = false; !isBranch; addr += kXLENInBytes) {
    auto binInst = getMem().loadWord(addr);
    auto inst = decoder_.decode(binInst);
    spdlog::trace(inst.str());
    bb.push_back(std::move(inst));
    isBranch = bb.back().isBranch;
  }

  spdlog::trace("Basic blok created.");
  return bb;
}

void Hart::run() {
  while (!state_.complete) {
    if (cache_.find(getPC()) == cache_.end())
      cache_[getPC()] = createBB(getPC());

    const auto &bb = cache_[getPC()];
    exec_.execute(bb.begin(), bb.end(), state_);
  }
}

} // namespace sim
