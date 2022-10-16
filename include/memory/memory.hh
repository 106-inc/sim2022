#ifndef __INCLUDE_MEMORY_MEMORY_HH__
#define __INCLUDE_MEMORY_MEMORY_HH__

#include "common/common.hh"

namespace sim {

class Memory final {
public:
  Word load(std::size_t);
};

} // namespace sim

#endif // __INCLUDE_MEMORY_MEMORY_HH__
