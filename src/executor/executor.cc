#include "executor/executor.hh"

namespace sim {

static auto applyOffset(RegVal val, RegVal imm) {
  int64_t res = static_cast<int64_t>(val) + signCast(imm);
  return static_cast<RegVal>(res);
}

static void executeAdd(const Instruction &inst, State &state) {
  RegVal rs1 = state.regs.get(inst.rs1);
  RegVal rs2 = state.regs.get(inst.rs2);
  state.regs.set(inst.rd, rs1 + rs2);
}

static void executeSub(const Instruction &inst, State &state) {
  RegVal rs1 = state.regs.get(inst.rs1);
  RegVal rs2 = state.regs.get(inst.rs2);
  state.regs.set(inst.rd, rs1 - rs2);
}

static void executeMul(const Instruction &inst, State &state) {
  RegVal rs1 = state.regs.get(inst.rs1);
  RegVal rs2 = state.regs.get(inst.rs2);
  state.regs.set(inst.rd, rs1 * rs2);
}

static void executeDiv(const Instruction &inst, State &state) {
  RegVal rs1 = state.regs.get(inst.rs1);
  RegVal rs2 = state.regs.get(inst.rs2);
  state.regs.set(inst.rd, rs1 / rs2);
}

static void executeLw(const Instruction &inst, State &state) {
  RegVal rs1 = state.regs.get(inst.rs1);
  state.regs.set(inst.rd, state.mem.loadWord(applyOffset(rs1, inst.imm)));
}

static void executeSw(const Instruction &inst, State &state) {
  RegVal rs1 = state.regs.get(inst.rs1);
  RegVal rs2 = state.regs.get(inst.rs1);
  state.mem.storeWord(applyOffset(rs1, inst.imm), rs2);
}

static void executeJAL(const Instruction &inst, State &state) {
  state.branchIsTaken = true;
  state.regs.set(inst.rd, state.pc + kXLENInBytes);
  state.npc = applyOffset(state.pc, inst.imm);
}

static void executeJALR(const Instruction &inst, State &state) {
  state.branchIsTaken = true;
  state.regs.set(inst.rd, state.pc + kXLENInBytes);
  RegVal rs1 = state.regs.get(inst.rs1);
  state.npc =
      setBit<0, 0>(applyOffset(rs1, inst.imm)); // setting the least-significant
                                                // bit of the result to zero.
}

Executor::Executor()
    : executors{{OpType::ADD, executeAdd}, {OpType::SUB, executeSub},
                {OpType::MUL, executeMul}, {OpType::DIV, executeDiv},
                {OpType::LW, executeLw},   {OpType::SW, executeSw},
                {OpType::JAL, executeJAL}, {OpType::JALR, executeJALR}} {}

void Executor::execute(const Instruction &inst, State &state) const {
  executors.at(inst.type)(inst, state);
}

} // namespace sim
