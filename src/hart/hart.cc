#include <spdlog/spdlog.h>

#include "elfloader/elfloader.hh"
#include "hart/hart.hh"

namespace sim {

Hart::Hart(const fs::path &executable) {
  ELFLoader loader{executable};
  pc() = loader.getEntryPoint();

  for (auto name : {".text", ".data", ".rodata"})
    if (loader.hasSection(name)) {
      auto text = loader.getSection(name);
      mem().storeRange(loader.getSectionAddr(name), text.begin(), text.end());
    }
}

void Hart::run() {
  while (!state_.complete) {
    auto binInst = mem().loadWord(pc());
    auto inst = decoder_.decode(binInst);
    spdlog::trace("Decoded instuction:\n  [0x{:08x}]{}", state_.pc, inst.str());

    exec_.execute(inst, state_);
    spdlog::trace("Current regfile state:\n{}", state_.regs.str());

    if (state_.branchIsTaken) {
      state_.pc = state_.npc;
      state_.branchIsTaken = false;
    } else {
      state_.pc += kXLENInBytes;
    }
  }
}

} // namespace sim
