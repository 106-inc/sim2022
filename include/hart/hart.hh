#ifndef __INCLUDE_HART_HART_HH__
#define __INCLUDE_HART_HART_HH__

#include <array>
#include <filesystem>
#include <memory>
#include <unordered_map>

#include "common/common.hh"
#include "common/inst.hh"
#include "common/state.hh"
#include "decoder/decoder.hh"
#include "executor/executor.hh"
#include "memory/memory.hh"

namespace sim {

namespace fs = std::filesystem;

class IBBCache {
public:
  virtual ~IBBCache() {}
  virtual const BasicBlock &
  lookupUpdate(Addr key, std::function<BasicBlock(Addr)> slowGetData) = 0;
};

class Hart final {
private:
  State state_{};
  Executor exec_{};
  Decoder decoder_{};
  std::unique_ptr<IBBCache> bbc_{};

  Memory &getMem() { return state_.mem; };
  Addr &getPC() { return state_.pc; };

  BasicBlock createBB(Addr entry);

public:
  Hart(const fs::path &executable, std::int64_t bbCacheSize);
  void run();
  std::uint64_t getInstrCount() const { return exec_.getInstrCount(); }
};

} // namespace sim

#endif // __INCLUDE_HART_HART_HH__
