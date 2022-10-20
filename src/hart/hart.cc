#include "hart/hart.hh"

namespace sim {

void Hart::run() {
  for (;;) {
    auto binInst = mem().loadWord(pc());
    auto inst = decoder_.decode(binInst);
    exec_.execute(inst, state_);
  }
}

} // namespace sim
