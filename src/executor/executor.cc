#include "executor/executor.hh"

namespace sim {

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
  RegVal offset = signCast(inst.imm);
  state.regs.set(inst.rd, state.mem.loadWord(rs1 + offset));
}

static void executeSw(const Instruction &inst, State &state) {
  RegVal address = state.regs.get(inst.rs1) + signCast(inst.imm);
  RegVal rs2 = state.regs.get(inst.rs1);
  state.mem.storeWord(address, rs2);
}

static void executeJAL(const Instruction &inst, State &state) {
  state.branchIsTaken = true;
  state.regs.set(inst.rd, state.pc + kXLENInBytes);
  state.npc = state.pc + signCast(inst.imm);
}

static void executeJALR(const Instruction &inst, State &state) {
  state.branchIsTaken = true;
  state.regs.set(inst.rd, state.pc + kXLENInBytes);
  RegVal rs1 = state.regs.get(inst.rs1);
  state.npc =
      setBit<0, 0>(rs1 + signCast(inst.imm)); // setting the least-significant
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
