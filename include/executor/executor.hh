#ifndef __INCLUDE_EXEC_EXEC_HH__
#define __INCLUDE_EXEC_EXEC_HH__

#include "common/inst.hh"
#include "common/state.hh"

namespace sim {

class Executor final {
public:
  void execute(const Instruction &inst, State &state) const;
};

} // namespace sim

#endif // __INCLUDE_EXEC_EXEC_HH__
