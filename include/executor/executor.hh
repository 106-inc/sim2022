#ifndef __INCLUDE_EXEC_EXEC_HH__
#define __INCLUDE_EXEC_EXEC_HH__

#include <functional>
#include <unordered_map>

#include "common/inst.hh"
#include "common/state.hh"

namespace sim {

class Executor final {
public:
  Executor();
  Executor(const Executor &) = delete;
  Executor(Executor &&) = delete;
  Executor &operator=(const Executor &) = delete;
  Executor &operator=(Executor &&) = delete;

  void execute(const Instruction &inst, State &state) const;

private:
  std::unordered_map<OpType, std::function<void(const Instruction, State &)>>
      executors;
};

} // namespace sim

#endif // __INCLUDE_EXEC_EXEC_HH__
