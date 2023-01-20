#include <algorithm>

#include "executor/executor.hh"

namespace sim {

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

void executeADD(const Instruction &inst, State &state) {
  executeRegisterRegisterOp(inst, state, std::plus<RegVal>());
}

void executeSUB(const Instruction &inst, State &state) {
  executeRegisterRegisterOp(inst, state, std::minus<RegVal>());
}

void executeMUL(const Instruction &inst, State &state) {
  executeRegisterRegisterOp(inst, state, std::multiplies<RegVal>());
}

void executeDIV(const Instruction &inst, State &state) {
  auto rs2 = state.regs.get(inst.rs2);
  if (rs2 == 0) {
    throw std::logic_error("division by zero");
  }
  executeRegisterRegisterOp(inst, state, std::divides<RegVal>());
}

void executeLW(const Instruction &inst, State &state) {
  auto rs1 = state.regs.get(inst.rs1);
  auto word = state.mem.loadWord(rs1 + inst.imm);
  state.regs.set(inst.rd, word);
}

void executeSW(const Instruction &inst, State &state) {
  auto rs1 = state.regs.get(inst.rs1);
  auto rs2 = state.regs.get(inst.rs2);
  state.mem.storeWord(rs1 + inst.imm, rs2);
}

void executeJAL(const Instruction &inst, State &state) {
  state.branchIsTaken = true;
  state.regs.set(inst.rd, state.pc + kXLENInBytes);
  state.npc = state.pc + inst.imm;
}

void executeJALR(const Instruction &inst, State &state) {
  state.branchIsTaken = true;
  state.regs.set(inst.rd, state.pc + kXLENInBytes);
  auto rs1 = state.regs.get(inst.rs1);
  state.npc = setBit<0, 0>(rs1 + inst.imm); // setting the least-significant
                                            // bit of the result to zero.
}

void executeECALL(const Instruction &, State &state) { state.complete = true; }

void executeADDI(const Instruction &inst, State &state) {
  executeRegisterImmidiateOp(inst, state, std::plus<RegVal>());
}

void executeANDI(const Instruction &inst, State &state) {
  executeRegisterImmidiateOp(inst, state, std::bit_and<RegVal>());
}

void executeORI(const Instruction &inst, State &state) {
  executeRegisterImmidiateOp(inst, state, std::bit_or<RegVal>());
}

void executeSLTI(const Instruction &inst, State &state) {
  auto rs1 = state.regs.get(inst.rs1);
  auto lhs = signCast(rs1);
  auto rhs = signCast(inst.imm);
  auto res = unsignedCast(executeSLT(lhs, rhs));
  state.regs.set(inst.rd, res);
}

void executeXORI(const Instruction &inst, State &state) {
  executeRegisterImmidiateOp(inst, state, std::bit_xor<RegVal>());
}

void executeSLTIU(const Instruction &inst, State &state) {
  auto rs1 = state.regs.get(inst.rs1);
  state.regs.set(inst.rd, executeSLT(rs1, inst.imm));
}

void executeLUI(const Instruction &inst, State &state) {
  state.regs.set(inst.rd, inst.imm << 12);
}

void executeAUIPC(const Instruction &inst, State &state) {
  state.regs.set(inst.rd, state.pc + (inst.imm << 12));
}

void executeSLLI(const Instruction &inst, State &state) {
  auto shamt = getBits<4, 0>(inst.imm);
  auto rs1 = state.regs.get(inst.rs1);
  state.regs.set(inst.rd, rs1 << shamt);
}

void executeSRLI(const Instruction &inst, State &state) {
  auto rs1 = state.regs.get(inst.rs1);
  auto res = executeSRLT(rs1, inst.imm);
  state.regs.set(inst.rd, res);
}
void executeSRAI(const Instruction &inst, State &state) {
  auto rs1 = state.regs.get(inst.rs1);
  auto signedRs1 = signCast(rs1);
  auto res = unsignedCast(executeSRLT(signedRs1, inst.imm));
  state.regs.set(inst.rd, res);
}

void executeSLL(const Instruction &inst, State &state) {
  auto rs1 = state.regs.get(inst.rs1);
  auto rs2 = state.regs.get(inst.rs2);
  state.regs.set(inst.rd, rs1 << rs2);
}

void executeSRL(const Instruction &inst, State &state) {
  executeRegisterRegisterOp(inst, state, executeSR<RegVal>);
}

void executeSRA(const Instruction &inst, State &state) {
  auto rs1 = signCast(state.regs.get(inst.rs1));
  auto rs2 = signCast(state.regs.get(inst.rs2));
  auto res = unsignedCast(executeSR(rs1, rs2));
  state.regs.set(inst.rd, res);
}

void executeBEQ(const Instruction &inst, State &state) {
  executeCondBranch(inst, state, std::equal_to<RegVal>());
}

void executeBNE(const Instruction &inst, State &state) {
  executeCondBranch(inst, state, std::not_equal_to<RegVal>());
}

void executeBLT(const Instruction &inst, State &state) {
  executeCondBranch(inst, state, std::less<RegVal>());
}

void executeBLTU(const Instruction &inst, State &state) {
  executeCondBranch(inst, state, std::greater<RegVal>());
}

void executeBGEU(const Instruction &inst, State &state) {
  executeCondBranch(inst, state, std::greater_equal<RegVal>());
}

void executeBGE(const Instruction &inst, State &state) {
  auto rs1 = signCast(state.regs.get(inst.rs1));
  auto rs2 = signCast(state.regs.get(inst.rs2));
  if (rs1 >= rs2) {
    state.branchIsTaken = true;
    state.npc = state.pc + inst.imm;
  }
}

void executeXOR(const Instruction &inst, State &state) {
  executeRegisterRegisterOp(inst, state, std::bit_xor<RegVal>());
}

void executeCSRRW(const Instruction &inst, State &state) {
  auto rs1 = state.regs.get(inst.rs1);
  auto csr = state.csregs.get(inst.csr);
  state.csregs.set(inst.csr, rs1);
  state.regs.set(inst.rd, csr);
}

void executeCSRRS(const Instruction &inst, State &state) {
  auto rs1 = state.regs.get(inst.rs1);
  auto csr = state.csregs.get(inst.csr);
  state.csregs.set(inst.csr, rs1 | csr);
  state.regs.set(inst.rd, csr);
}

void executeCSRRC(const Instruction &inst, State &state) {
  auto rs1 = state.regs.get(inst.rs1);
  auto csr = state.csregs.get(inst.csr);
  state.csregs.set(inst.csr, rs1 & (~csr));
  state.regs.set(inst.rd, csr);
}

void executeCSRRWI(const Instruction &inst, State &state) {
  auto rs1 = inst.rs1;
  auto csr = state.csregs.get(inst.csr);
  state.regs.set(inst.rd, csr);
  state.csregs.set(inst.csr, getBits<4, 0>(rs1));
}

void executeCSRRSI(const Instruction &inst, State &state) {
  auto rs1 = inst.rs1;
  auto csr = state.csregs.get(inst.csr);
  state.regs.set(inst.rd, csr);
  state.csregs.set(inst.csr, csr | getBits<4, 0>(rs1));
}

void executeCSRRCI(const Instruction &inst, State &state) {
  auto rs1 = inst.rs1;
  auto csr = state.csregs.get(inst.csr);
  state.regs.set(inst.rd, csr);
  state.csregs.set(inst.csr, csr & (~getBits<4, 0>(rs1)));
}

[[noreturn]] void executeAND(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeDIVU(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFEQ_D(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFEQ_S(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFLE_D(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFLE_S(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFLT_D(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFLT_S(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFMAX_D(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFMAX_S(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFMIN_D(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFMIN_S(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFSGNJ_D(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFSGNJ_S(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFSGNJN_D(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFSGNJN_S(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFSGNJX_D(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFSGNJX_S(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeMULH(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeMULHSU(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeMULHU(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeOR(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeREM(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeREMU(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeSLT(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeSLTU(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFENCE(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFLD(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFLW(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFSD(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFSW(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeLB(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeLBU(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeLH(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeLHU(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeSB(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeSH(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeAMOADD_W(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeAMOAND_W(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeAMOMAX_W(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeAMOMAXU_W(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeAMOMIN_W(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeAMOMINU_W(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeAMOOR_W(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeAMOSWAP_W(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeAMOXOR_W(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeSC_W(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeEBREAK(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFADD_D(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFADD_S(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFDIV_D(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFDIV_S(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFMUL_D(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFMUL_S(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFSUB_D(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFSUB_S(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFCLASS_D(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFCLASS_S(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFMV_W_X(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFMV_X_W(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFCVT_D_S(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFCVT_D_W(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFCVT_D_WU(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFCVT_S_D(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFCVT_S_W(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFCVT_S_WU(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFCVT_W_D(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFCVT_W_S(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFCVT_WU_D(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFCVT_WU_S(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFSQRT_D(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFSQRT_S(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFMADD_D(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFMADD_S(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFMSUB_D(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFMSUB_S(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFNMADD_D(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFNMADD_S(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFNMSUB_D(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeFNMSUB_S(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

[[noreturn]] void executeLR_W(const Instruction &, State &) {
  throw std::runtime_error{"Not implemented yet"};
}

} // namespace sim
