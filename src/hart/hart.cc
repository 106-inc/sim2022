#include "hart/hart.hh"

namespace sim {

void Hart::run() {

  size_t i = 0;
  for (;i < 100; ++i) {
    auto binInst = mem().load(pc());
    auto inst = decoder_.decode(binInst);
    exec_.execute(inst, state_);
  }
}

} // namespace sim
