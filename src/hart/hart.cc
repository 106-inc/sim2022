#include "hart/hart.hh"
#include "elfloader/elfloader.hh"

namespace sim {

Hart::Hart(const fs::path &executable) {
  ELFLoader loader{executable};
  pc() = loader.getEntryPoint();

  auto text = loader.getSection(".text");
  mem().storeRange(loader.getSectionAddr(".text"), text.begin(), text.end());

  // auto data = loader.getSection(".data");
  // mem().storeRange(loader.getSectionAddr(".data"), data.begin(), data.end());

  // auto rodata = loader.getSection(".rodata");
  // mem().storeRange(loader.getSectionAddr(".rodata"), rodata.begin(),
  //                  rodata.end());
}

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
