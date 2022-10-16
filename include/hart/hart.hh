#ifndef __INCLUDE_HART_HART_HH__
#define __INCLUDE_HART_HART_HH__

#include <array>

#include "common/common.hh"
#include "common/state.hh"
#include "decoder/decoder.hh"
#include "executor/executor.hh"
#include "memory/memory.hh"

namespace sim {

class Hart final {
private:
  State state_;
  Executor exec_;
  Decoder decoder_;

  Memory &mem() { return state_.mem; };
  Word &pc() { return state_.pc; };

public:
  void run();
};

} // namespace sim

#endif // __INCLUDE_HART_HART_HH__
