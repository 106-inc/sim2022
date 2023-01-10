#include <algorithm>

#include "executor/executor.hh"

namespace sim {

void Executor::execute(const Instruction &inst, State &state) {
  execMap_.at(inst.type)(inst, state);
  if (state.branchIsTaken) {
    state.pc = state.npc;
    state.branchIsTaken = false;
  } else {
    state.pc += kXLENInBytes;
  }
}

template <std::regular_invocable<RegVal, RegVal> Func>
void executeRegisterRegisterOp(const Instruction &inst, State &state, Func op) {
  auto rs1 = state.regs.get(inst.rs1);
  auto rs2 = state.regs.get(inst.rs2);
  state.regs.set(inst.rd, op(rs1, rs2));
}

template <std::regular_invocable<RegVal, RegVal> Func>
void executeRegisterImmidiateOp(const Instruction &inst, State &state,
                                Func op) {
  auto rs1 = state.regs.get(inst.rs1);
  state.regs.set(inst.rd, op(rs1, inst.imm));
}

template <std::integral T> static T executeSLT(T lhs, T rhs) {
  return lhs < rhs;
}

template <std::integral T> static T executeSRLT(T val, RegVal imm) {
  RegVal shamt = getBits<4, 0>(imm);
  return val >> shamt;
}

template <std::integral T> static T executeSR(T lhs, T rhs) {
  return lhs >> rhs;
}

template <std::predicate<RegVal, RegVal> Func>
void executeCondBranch(const Instruction &inst, State &state, Func op) {
  auto rs1 = state.regs.get(inst.rs1);
  auto rs2 = state.regs.get(inst.rs2);
  if (op(rs1, rs2)) {
    state.branchIsTaken = true;
    state.npc = state.pc + inst.imm;
  }
}

#define SIM2022_MAKE_EXEC_DEF(op, action)                                      \
  static void execute##op([[maybe_unused]] const Instruction &inst,            \
                          State &state) {                                      \
    action                                                                     \
  }

#include "executor.ii"

#undef SIM2022_MAKE_EXEC_DEF

const Executor::ExecutorMap Executor::execMap_{
#define SIM2022_MAKE_EXEC_DEF(op, action) {OpType::op, execute##op},

#include "executor.ii"

#undef SIM2022_MAKE_EXEC_DEF
};

} // namespace sim
