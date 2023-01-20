#ifndef __INCLUDE_EXECUTOR_EXECUTOR_HH__
#define __INCLUDE_EXECUTOR_EXECUTOR_HH__

#include <concepts>
#include <functional>
#include <iterator>
#include <unordered_map>

#include <spdlog/spdlog.h>

#include "common/inst.hh"
#include "common/state.hh"

namespace sim {

template <typename T>
concept InstForwardIterator = std::input_iterator<T> &&
    std::is_same_v<typename std::iterator_traits<T>::value_type, Instruction>;

class Executor final {
public:
  Executor() = default;
  Executor(const Executor &) = delete;
  Executor(Executor &&) = delete;
  Executor &operator=(const Executor &) = delete;
  Executor &operator=(Executor &&) = delete;

  void execute(const Instruction &inst, State &state);

  template <InstForwardIterator It>
  void execute(It begin, It end, State &state) {
    std::for_each(begin, end, [this, &state](const auto &inst) {
      cosimLog("-----------------------");
      cosimLog("NUM={}", instrCount);
      this->execute(inst, state);
      cosimLog("PC=0x{:08x}", state.pc);
      spdlog::trace("Instruction:\n  [0x{:08x}]{}", state.pc, inst.str());
      spdlog::trace("Current regfile state:\n{}", state.regs.str());
      this->instrCount += 1;
      state.csregs.updateTimers(inst.type);
    });
  }

  std::size_t getInstrCount() const { return instrCount; }

private:
  static const std::unordered_map<
      OpType, std::function<void(const Instruction, State &)>>
      execMap_;
  std::size_t instrCount{1};
};

} // namespace sim

#endif // __INCLUDE_EXECUTOR_EXECUTOR_HH__
