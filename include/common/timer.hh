#ifndef __INCLUDE_COMMON_TIMER_HH__
#define __INCLUDE_COMMON_TIMER_HH__

#include <chrono>

namespace timer {
class Timer final {
private:
  std::chrono::high_resolution_clock::time_point start;

public:
  Timer() : start(std::chrono::high_resolution_clock::now()) {}

  void reset() { start = std::chrono::high_resolution_clock::now(); }

  [[nodiscard]] auto elapsedMs() {
    auto end = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
        .count();
  }

  [[nodiscard]] auto elapsedMcs() {
    auto end = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(end - start)
        .count();
  }
};

} // namespace timer

#endif // __INCLUDE_COMMON_TIMER_HH__
