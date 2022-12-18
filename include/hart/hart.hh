#ifndef __INCLUDE_HART_HART_HH__
#define __INCLUDE_HART_HART_HH__

#include <array>
#include <filesystem>
#include <unordered_map>

#include "common/common.hh"
#include "common/inst.hh"
#include "common/state.hh"
#include "decoder/decoder.hh"
#include "executor/executor.hh"
#include "memory/memory.hh"

namespace sim {

namespace fs = std::filesystem;

using BBCache = std::unordered_map<Addr, BasicBlock>;

class Hart final {
private:
  State state_{};
  Executor exec_{};
  Decoder decoder_{};
  BBCache cache_{};

  Memory &getMem() { return state_.mem; };
  Addr &getPC() { return state_.pc; };

  BasicBlock createBB(Addr entry);

public:
  Hart(const fs::path &executable);
  void run();
};

} // namespace sim

#endif // __INCLUDE_HART_HART_HH__
