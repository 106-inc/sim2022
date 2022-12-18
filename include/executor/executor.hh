#ifndef __INCLUDE_EXECUTOR_EXECUTOR_HH__
#define __INCLUDE_EXECUTOR_EXECUTOR_HH__

#include <concepts>
#include <functional>
#include <iterator>
#include <unordered_map>

#include "common/inst.hh"
#include "common/state.hh"

namespace sim {

template <typename T>
concept InstForwardIterator = std::forward_iterator<T> &&
    std::is_same_v<Instruction, typename std::iterator_traits<T>::value_type>;

class Executor final {
public:
  Executor() = default;
  Executor(const Executor &) = delete;
  Executor(Executor &&) = delete;
  Executor &operator=(const Executor &) = delete;
  Executor &operator=(Executor &&) = delete;

  void execute(const Instruction &inst, State &state) const;

  template <InstForwardIterator It>
  void execute(It begin, It end, State &state) const;

private:
  static const std::unordered_map<
      OpType, std::function<void(const Instruction, State &)>>
      execMap_;
};

} // namespace sim

#endif // __INCLUDE_EXECUTOR_EXECUTOR_HH__
