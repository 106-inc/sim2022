#include "executor/executor.hh"

namespace sim {

static auto applyOffset(RegVal val, RegVal imm) {
  auto res = signAdd(val, imm);

  if (signCast(res) < 0)
    throw std::runtime_error("invalid address");

  return res;
}

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

static void executeSLTI(const Instruction &inst, State &state) {
  RegVal rs1 = state.regs.get(inst.rs1);
  state.regs.set(inst.rd, rs1 < inst.imm);
}

inline static void executeSLTIU(const Instruction &inst, State &state) {
  // instruction have the same semantic as SLTI.
  // Main difference is result interpretation.
  executeSLTI(inst, state);
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

static void executeSRLI(const Instruction &inst, State &state) {
  RegVal shamt = getBits<5, 0>(inst.imm);
  RegVal rs1 = state.regs.get(inst.rs1);
  state.regs.set(inst.rd, rs1 >> shamt);
}

static void executeSRAI(const Instruction &inst, State &state) {
  // instruction have the same semantic as SRLI.
  // Main difference is result interpretation.
  executeSRLI(inst, state);
}

static void executeSLL(const Instruction &inst, State &state) {
  RegVal rs1 = state.regs.get(inst.rs1);
  RegVal rs2 = state.regs.get(inst.rs2);
  state.regs.set(inst.rd, rs1 << rs2);
}

static void executeSRL(const Instruction &inst, State &state) {
  RegVal rs1 = state.regs.get(inst.rs1);
  RegVal rs2 = state.regs.get(inst.rs2);
  state.regs.set(inst.rd, rs1 >> rs2);
}

static void executeSRA(const Instruction &inst, State &state) {
  // instruction have the same semantic as SRL.
  // Main difference is result interpretation.
  executeSRL(inst, state);
}

Executor::Executor()
    : executors_{{OpType::ADD, executeADD},    {OpType::SUB, executeSUB},
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
                {OpType::SRL, executeSRL},     {OpType::SRA, executeSRA}} {}

void Executor::execute(const Instruction &inst, State &state) const {
  executors_.at(inst.type)(inst, state);
}

} // namespace sim
