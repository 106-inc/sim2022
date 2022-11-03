#include "hart/hart.hh"

namespace sim {

void Hart::run() {
  while (!state_.complete) {
    auto binInst = mem().loadWord(pc());
    auto inst = decoder_.decode(binInst);
    exec_.execute(inst, state_);
    if (state_.branchIsTaken) {
      state_.pc = state_.npc;
      state_.branchIsTaken = false;
    } else {
      state_.pc += kXLENInBytes;
    }
  }
}

} // namespace sim
