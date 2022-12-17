#include "executor/executor.hh"

namespace sim {

static void executeADD(const Instruction &inst, State &state) {
  RegVal rs1 = state.regs.get(inst.rs1);
  RegVal rs2 = state.regs.get(inst.rs2);
  state.regs.set(inst.rd, rs1 + rs2);
}

static void executeSUB(const Instruction &inst, State &state) {
  RegVal rs1 = state.regs.get(inst.rs1);
  RegVal rs2 = state.regs.get(inst.rs2);
  state.regs.set(inst.rd, rs1 - rs2);
}

static void executeMUL(const Instruction &inst, State &state) {
  RegVal rs1 = state.regs.get(inst.rs1);
  RegVal rs2 = state.regs.get(inst.rs2);
  state.regs.set(inst.rd, rs1 * rs2);
}

static void executeDIV(const Instruction &inst, State &state) {
  RegVal rs1 = state.regs.get(inst.rs1);
  RegVal rs2 = state.regs.get(inst.rs2);
  state.regs.set(inst.rd, rs1 / rs2);
}

static void executeLW(const Instruction &inst, State &state) {
  RegVal rs1 = state.regs.get(inst.rs1);
  RegVal word = state.mem.loadWord(rs1 + inst.imm);
  state.regs.set(inst.rd, word);
}

static void executeSW(const Instruction &inst, State &state) {
  RegVal rs1 = state.regs.get(inst.rs1);
  RegVal rs2 = state.regs.get(inst.rs2);
  state.mem.storeWord(rs1 + inst.imm, rs2);
}

static void executeJAL(const Instruction &inst, State &state) {
  state.branchIsTaken = true;
  state.regs.set(inst.rd, state.pc + kXLENInBytes);
  state.npc = state.pc + inst.imm;
}

static void executeJALR(const Instruction &inst, State &state) {
  state.branchIsTaken = true;
  state.regs.set(inst.rd, state.pc + kXLENInBytes);
  RegVal rs1 = state.regs.get(inst.rs1);
  state.npc = setBit<0, 0>(rs1 + inst.imm); // setting the least-significant
                                            // bit of the result to zero.
}

static void executeECALL(const Instruction &, State &state) {
  state.complete = true;
}

static void executeADDI(const Instruction &inst, State &state) {
  RegVal rs1 = state.regs.get(inst.rs1);
  state.regs.set(inst.rd, rs1 + inst.imm);
}

template <std::integral T> static T executeSLT(T lhs, T rhs) {
  return lhs < rhs;
}

static void executeSLTI(const Instruction &inst, State &state) {
  RegVal rs1 = state.regs.get(inst.rs1);
  auto &&lhs = signCast(rs1);
  auto &&rhs = signCast(inst.imm);
  RegVal res = unsignedCast(executeSLT(lhs, rhs));
  state.regs.set(inst.rd, res);
}

inline static void executeSLTIU(const Instruction &inst, State &state) {
  RegVal rs1 = state.regs.get(inst.rs1);
  state.regs.set(inst.rd, executeSLT(rs1, inst.imm));
}

static void executeANDI(const Instruction &inst, State &state) {
  RegVal rs1 = state.regs.get(inst.rs1);
  state.regs.set(inst.rd, rs1 & inst.imm);
}

static void executeORI(const Instruction &inst, State &state) {
  RegVal rs1 = state.regs.get(inst.rs1);
  state.regs.set(inst.rd, rs1 | inst.imm);
}

static void executeXORI(const Instruction &inst, State &state) {
  RegVal rs1 = state.regs.get(inst.rs1);
  state.regs.set(inst.rd, rs1 ^ inst.imm);
}

static void executeLUI(const Instruction &inst, State &state) {
  state.regs.set(inst.rd, inst.imm << 12);
}

static void executeAUIPC(const Instruction &inst, State &state) {
  state.regs.set(inst.rd, state.pc + (inst.imm << 12));
}

static void executeSLLI(const Instruction &inst, State &state) {
  RegVal shamt = getBits<5, 0>(inst.imm);
  RegVal rs1 = state.regs.get(inst.rs1);
  state.regs.set(inst.rd, rs1 << shamt);
}

template <std::integral T> static T executeSRLT(T val, RegVal imm) {
  RegVal shamt = getBits<5, 0>(imm);
  return val >> shamt;
}

static void executeSRLI(const Instruction &inst, State &state) {
  RegVal rs1 = state.regs.get(inst.rs1);
  auto &&signedRs1 = signCast(rs1);
  RegVal res = unsignedCast(executeSRLT(signedRs1, inst.imm));
  state.regs.set(inst.rd, res);
}

static void executeSRAI(const Instruction &inst, State &state) {
  RegVal rs1 = state.regs.get(inst.rs1);
  RegVal res = executeSRLT(rs1, inst.imm);
  state.regs.set(inst.rd, res);
}

static void executeSLL(const Instruction &inst, State &state) {
  RegVal rs1 = state.regs.get(inst.rs1);
  RegVal rs2 = state.regs.get(inst.rs2);
  state.regs.set(inst.rd, rs1 << rs2);
}

template <std::integral T> static T executeSR(T lhs, T rhs) {
  return lhs >> rhs;
}

static void executeSRL(const Instruction &inst, State &state) {
  auto &&rs1 = signCast(state.regs.get(inst.rs1));
  auto &&rs2 = signCast(state.regs.get(inst.rs2));
  RegVal res = unsignedCast(executeSR(rs1, rs2));
  state.regs.set(inst.rd, res);
}

static void executeSRA(const Instruction &inst, State &state) {
  RegVal rs1 = state.regs.get(inst.rs1);
  RegVal rs2 = state.regs.get(inst.rs2);
  state.regs.set(inst.rd, executeSR(rs1, rs2));
}

static void executeBEQ(const Instruction &inst, State &state) {
  RegVal rs1 = state.regs.get(inst.rs1);
  RegVal rs2 = state.regs.get(inst.rs2);
  if (rs1 == rs2) {
    state.branchIsTaken = true;
    state.npc = inst.imm;
  }
}

static void executeBNE(const Instruction &inst, State &state) {
  RegVal rs1 = state.regs.get(inst.rs1);
  RegVal rs2 = state.regs.get(inst.rs2);
  if (rs1 != rs2) {
    state.branchIsTaken = true;
    state.npc = inst.imm;
  }
}

static void executeBLT(const Instruction &inst, State &state) {
  auto rs1 = signCast(state.regs.get(inst.rs1));
  auto rs2 = signCast(state.regs.get(inst.rs2));
  if (rs1 < rs2) {
    state.branchIsTaken = true;
    state.npc = inst.imm;
  }
}

static void executeBGE(const Instruction &inst, State &state) {
  auto rs1 = signCast(state.regs.get(inst.rs1));
  auto rs2 = signCast(state.regs.get(inst.rs2));
  if (rs1 >= rs2) {
    state.branchIsTaken = true;
    state.npc = inst.imm;
  }
}

static void executeBLTU(const Instruction &inst, State &state) {
  RegVal rs1 = state.regs.get(inst.rs1);
  RegVal rs2 = state.regs.get(inst.rs2);
  if (rs1 > rs2) {
    state.branchIsTaken = true;
    state.npc = inst.imm;
  }
}

static void executeBGEU(const Instruction &inst, State &state) {
  RegVal rs1 = state.regs.get(inst.rs1);
  RegVal rs2 = state.regs.get(inst.rs2);
  if (rs1 >= rs2) {
    state.branchIsTaken = true;
    state.npc = inst.imm;
  }
}

Executor::Executor()
    : executors_{{OpType::ADD, executeADD},     {OpType::SUB, executeSUB},
                 {OpType::MUL, executeMUL},     {OpType::DIV, executeDIV},
                 {OpType::LW, executeLW},       {OpType::SW, executeSW},
                 {OpType::JAL, executeJAL},     {OpType::JALR, executeJALR},
                 {OpType::ECALL, executeECALL}, {OpType::ADDI, executeADDI},
                 {OpType::SLTI, executeSLTI},   {OpType::SLTIU, executeSLTIU},
                 {OpType::ANDI, executeANDI},   {OpType::ORI, executeORI},
                 {OpType::XORI, executeXORI},   {OpType::LUI, executeLUI},
                 {OpType::AUIPC, executeAUIPC}, {OpType::SLLI, executeSLLI},
                 {OpType::SRLI, executeSRLI},   {OpType::SRAI, executeSRAI},
                 {OpType::SLL, executeSLL},     {OpType::SLL, executeSLL},
                 {OpType::SRL, executeSRL},     {OpType::SRA, executeSRA},
                 {OpType::BEQ, executeBEQ},     {OpType::BNE, executeBNE},
                 {OpType::BLT, executeBLT},     {OpType::BGE, executeBGE},
                 {OpType::BLTU, executeBLTU},   {OpType::BGEU, executeBGEU}} {}

void Executor::execute(const Instruction &inst, State &state) const {
  executors_.at(inst.type)(inst, state);
}

} // namespace sim
