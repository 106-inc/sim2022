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

  void execute(const Instruction &inst, State &state) {
    inst.callback(inst, state);
    if (state.branchIsTaken) {
      state.pc = state.npc;
      state.branchIsTaken = false;
    } else {
      state.pc += kXLENInBytes;
    }
  }

  template <InstForwardIterator It>
  void execute(It begin, It end, State &state) {
    std::for_each(begin, end, [this, &state](const auto &inst) {
#ifdef SPDLOG
      cosimLog("-----------------------");
      cosimLog("NUM={}", instrCount);
#endif
      this->execute(inst, state);
#ifdef SPDLOG
      cosimLog("PC=0x{:08x}", state.pc);
      spdlog::trace("Instruction:\n  [0x{:08x}]{}", state.pc, inst.str());
      spdlog::trace("Current regfile state:\n{}", state.regs.str());
#endif
      this->instrCount += 1;
      state.csregs.updateTimers(inst.type);
    });
  }

  std::uint64_t getInstrCount() const { return instrCount; }

private:
  std::uint64_t instrCount{1};
};

void executeADD(const Instruction &inst, State &state);
void executeSUB(const Instruction &inst, State &state);
void executeMUL(const Instruction &inst, State &state);
void executeDIV(const Instruction &inst, State &state);
void executeLW(const Instruction &inst, State &state);
void executeSW(const Instruction &inst, State &state);
void executeJAL(const Instruction &inst, State &state);
void executeJALR(const Instruction &inst, State &state);
void executeECALL(const Instruction &inst, State &state);
void executeADDI(const Instruction &inst, State &state);
void executeANDI(const Instruction &inst, State &state);
void executeORI(const Instruction &inst, State &state);
void executeSLTI(const Instruction &inst, State &state);
void executeXORI(const Instruction &inst, State &state);
void executeSLTIU(const Instruction &inst, State &state);
void executeLUI(const Instruction &inst, State &state);
void executeAUIPC(const Instruction &inst, State &state);
void executeSLLI(const Instruction &inst, State &state);
void executeSRLI(const Instruction &inst, State &state);
void executeSRAI(const Instruction &inst, State &state);
void executeSLL(const Instruction &inst, State &state);
void executeSRL(const Instruction &inst, State &state);
void executeSRA(const Instruction &inst, State &state);
void executeBEQ(const Instruction &inst, State &state);
void executeBNE(const Instruction &inst, State &state);
void executeBLT(const Instruction &inst, State &state);
void executeBLTU(const Instruction &inst, State &state);
void executeBGEU(const Instruction &inst, State &state);
void executeBGE(const Instruction &inst, State &state);
void executeXOR(const Instruction &inst, State &state);
void executeCSRRW(const Instruction &inst, State &state);
void executeCSRRS(const Instruction &inst, State &state);
void executeCSRRC(const Instruction &inst, State &state);
void executeCSRRWI(const Instruction &inst, State &state);
void executeCSRRSI(const Instruction &inst, State &state);
void executeCSRRCI(const Instruction &inst, State &state);

/* unrealized */

void executeAND(const Instruction &inst, State &state);
void executeDIVU(const Instruction &inst, State &state);
void executeFEQ_D(const Instruction &inst, State &state);
void executeFEQ_S(const Instruction &inst, State &state);
void executeFLE_D(const Instruction &inst, State &state);
void executeFLE_S(const Instruction &inst, State &state);
void executeFLT_D(const Instruction &inst, State &state);
void executeFLT_S(const Instruction &inst, State &state);
void executeFMAX_D(const Instruction &inst, State &state);
void executeFMAX_S(const Instruction &inst, State &state);
void executeFMIN_D(const Instruction &inst, State &state);
void executeFMIN_S(const Instruction &inst, State &state);
void executeFSGNJ_D(const Instruction &inst, State &state);
void executeFSGNJ_S(const Instruction &inst, State &state);
void executeFSGNJN_D(const Instruction &inst, State &state);
void executeFSGNJN_S(const Instruction &inst, State &state);
void executeFSGNJX_D(const Instruction &inst, State &state);
void executeFSGNJX_S(const Instruction &inst, State &state);
void executeMULH(const Instruction &inst, State &state);
void executeMULHSU(const Instruction &inst, State &state);
void executeMULHU(const Instruction &inst, State &state);
void executeOR(const Instruction &inst, State &state);
void executeREM(const Instruction &inst, State &state);
void executeREMU(const Instruction &inst, State &state);
void executeSLT(const Instruction &inst, State &state);
void executeSLTU(const Instruction &inst, State &state);
void executeFENCE(const Instruction &inst, State &state);
void executeFLD(const Instruction &inst, State &state);
void executeFLW(const Instruction &inst, State &state);
void executeFSD(const Instruction &inst, State &state);
void executeFSW(const Instruction &inst, State &state);
void executeLB(const Instruction &inst, State &state);
void executeLBU(const Instruction &inst, State &state);
void executeLH(const Instruction &inst, State &state);
void executeLHU(const Instruction &inst, State &state);
void executeSB(const Instruction &inst, State &state);
void executeSH(const Instruction &inst, State &state);
void executeAMOADD_W(const Instruction &inst, State &state);
void executeAMOAND_W(const Instruction &inst, State &state);
void executeAMOMAX_W(const Instruction &inst, State &state);
void executeAMOMAXU_W(const Instruction &inst, State &state);
void executeAMOMIN_W(const Instruction &inst, State &state);
void executeAMOMINU_W(const Instruction &inst, State &state);
void executeAMOOR_W(const Instruction &inst, State &state);
void executeAMOSWAP_W(const Instruction &inst, State &state);
void executeAMOXOR_W(const Instruction &inst, State &state);
void executeSC_W(const Instruction &inst, State &state);
void executeEBREAK(const Instruction &inst, State &state);
void executeFADD_D(const Instruction &inst, State &state);
void executeFADD_S(const Instruction &inst, State &state);
void executeFDIV_D(const Instruction &inst, State &state);
void executeFDIV_S(const Instruction &inst, State &state);
void executeFMUL_D(const Instruction &inst, State &state);
void executeFMUL_S(const Instruction &inst, State &state);
void executeFSUB_D(const Instruction &inst, State &state);
void executeFSUB_S(const Instruction &inst, State &state);
void executeFCLASS_D(const Instruction &inst, State &state);
void executeFCLASS_S(const Instruction &inst, State &state);
void executeFMV_W_X(const Instruction &inst, State &state);
void executeFMV_X_W(const Instruction &inst, State &state);
void executeFCVT_D_S(const Instruction &inst, State &state);
void executeFCVT_D_W(const Instruction &inst, State &state);
void executeFCVT_D_WU(const Instruction &inst, State &state);
void executeFCVT_S_D(const Instruction &inst, State &state);
void executeFCVT_S_W(const Instruction &inst, State &state);
void executeFCVT_S_WU(const Instruction &inst, State &state);
void executeFCVT_W_D(const Instruction &inst, State &state);
void executeFCVT_W_S(const Instruction &inst, State &state);
void executeFCVT_WU_D(const Instruction &inst, State &state);
void executeFCVT_WU_S(const Instruction &inst, State &state);
void executeFSQRT_D(const Instruction &inst, State &state);
void executeFSQRT_S(const Instruction &inst, State &state);
void executeFMADD_D(const Instruction &inst, State &state);
void executeFMADD_S(const Instruction &inst, State &state);
void executeFMSUB_D(const Instruction &inst, State &state);
void executeFMSUB_S(const Instruction &inst, State &state);
void executeFNMADD_D(const Instruction &inst, State &state);
void executeFNMADD_S(const Instruction &inst, State &state);
void executeFNMSUB_D(const Instruction &inst, State &state);
void executeFNMSUB_S(const Instruction &inst, State &state);
void executeLR_W(const Instruction &inst, State &state);

} // namespace sim

#endif // __INCLUDE_EXECUTOR_EXECUTOR_HH__
