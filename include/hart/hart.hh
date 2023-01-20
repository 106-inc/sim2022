#ifndef __INCLUDE_HART_HART_HH__
#define __INCLUDE_HART_HART_HH__

#include <array>
#include <filesystem>
#include <functional>
#include <iterator>
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

// template <typename T, typename KeyT> class LRUCache final {
//   std::size_t size_;
//   std::list<std::pair<KeyT, T>> cache_;

//   using ListIt = typename std::list<std::pair<KeyT, T>>::iterator;
//   std::unordered_map<KeyT, ListIt> hash_;

// public:
//   LRUCache(std::size_t size) : size_(size) {}

//   bool isFull() const { return (cache_.size() == size_); }

//   const T &lookupUpdate(KeyT key, std::function<T(KeyT)> slowGetData) {
//     auto hit = hash_.find(key);
//     if (hit == hash_.end()) {
//       if (isFull()) {
//         hash_.erase(cache_.back().first);
//         cache_.pop_back();
//       }
//       cache_.emplace_front(key, slowGetData(key));
//       hash_.emplace(key, cache_.begin());
//       return cache_.front();
//     }

//     if (auto eltit = hit->second; eltit != cache_.begin())
//       cache_.splice(cache_.begin(), cache_, eltit, std::next(eltit));

//     return *hit;
//   }
// };

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
  Hart(const fs::path &executable, std::int64_t bbCacheSize);
  void run();
};

} // namespace sim

#endif // __INCLUDE_HART_HART_HH__
