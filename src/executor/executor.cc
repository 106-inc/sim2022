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

const std::unordered_map<OpType,
                         std::function<void(const Instruction, State &)>>
    Executor::execMap_{
        {OpType::ADD,
         [](const Instruction &inst, State &state) {
           executeRegisterRegisterOp(inst, state, std::plus<RegVal>());
         }},
        {OpType::SUB,
         [](const Instruction &inst, State &state) {
           executeRegisterRegisterOp(inst, state, std::minus<RegVal>());
         }},
        {OpType::MUL,
         [](const Instruction &inst, State &state) {
           executeRegisterRegisterOp(inst, state, std::multiplies<RegVal>());
         }},
        {OpType::DIV,
         [](const Instruction &inst, State &state) {
           auto rs2 = state.regs.get(inst.rs2);
           if (rs2 == 0) {
             throw std::logic_error("division by zero");
           }
           executeRegisterRegisterOp(inst, state, std::divides<RegVal>());
         }},
        {OpType::LW,
         [](const Instruction &inst, State &state) {
           auto rs1 = state.regs.get(inst.rs1);
           auto word = state.mem.loadWord(rs1 + inst.imm);
           state.regs.set(inst.rd, word);
         }},
        {OpType::SW,
         [](const Instruction &inst, State &state) {
           auto rs1 = state.regs.get(inst.rs1);
           auto rs2 = state.regs.get(inst.rs2);
           state.mem.storeWord(rs1 + inst.imm, rs2);
         }},
        {OpType::JAL,
         [](const Instruction &inst, State &state) {
           state.branchIsTaken = true;
           state.regs.set(inst.rd, state.pc + kXLENInBytes);
           state.npc = state.pc + inst.imm;
         }},
        {OpType::JALR,
         [](const Instruction &inst, State &state) {
           state.branchIsTaken = true;
           state.regs.set(inst.rd, state.pc + kXLENInBytes);
           auto rs1 = state.regs.get(inst.rs1);
           state.npc =
               setBit<0, 0>(rs1 + inst.imm); // setting the least-significant
                                             // bit of the result to zero.
         }},
        {OpType::ECALL,
         [](const Instruction &, State &state) { state.complete = true; }},
        {OpType::ADDI,
         [](const Instruction &inst, State &state) {
           executeRegisterImmidiateOp(inst, state, std::plus<RegVal>());
         }},
        {OpType::ANDI,
         [](const Instruction &inst, State &state) {
           executeRegisterImmidiateOp(inst, state, std::bit_and<RegVal>());
         }},

        {OpType::XORI,
         [](const Instruction &inst, State &state) {
           executeRegisterImmidiateOp(inst, state, std::bit_xor<RegVal>());
         }},

        {OpType::ORI,
         [](const Instruction &inst, State &state) {
           executeRegisterImmidiateOp(inst, state, std::bit_or<RegVal>());
         }},
        {OpType::SLTI,
         [](const Instruction &inst, State &state) {
           auto rs1 = state.regs.get(inst.rs1);
           auto lhs = signCast(rs1);
           auto rhs = signCast(inst.imm);
           auto res = unsignedCast(executeSLT(lhs, rhs));
           state.regs.set(inst.rd, res);
         }},
        {OpType::SLTIU,
         [](const Instruction &inst, State &state) {
           auto rs1 = state.regs.get(inst.rs1);
           state.regs.set(inst.rd, executeSLT(rs1, inst.imm));
         }},

        {OpType::LUI,
         [](const Instruction &inst, State &state) {
           state.regs.set(inst.rd, inst.imm << 12);
         }},
        {OpType::AUIPC,
         [](const Instruction &inst, State &state) {
           state.regs.set(inst.rd, state.pc + (inst.imm << 12));
         }},

        {OpType::SLLI,
         [](const Instruction &inst, State &state) {
           auto shamt = getBits<4, 0>(inst.imm);
           auto rs1 = state.regs.get(inst.rs1);
           state.regs.set(inst.rd, rs1 << shamt);
         }},
        {OpType::SRLI,
         [](const Instruction &inst, State &state) {
           auto rs1 = state.regs.get(inst.rs1);
           auto res = executeSRLT(rs1, inst.imm);
           state.regs.set(inst.rd, res);
         }},
        {OpType::SRAI,
         [](const Instruction &inst, State &state) {
           auto rs1 = state.regs.get(inst.rs1);
           auto signedRs1 = signCast(rs1);
           auto res = unsignedCast(executeSRLT(signedRs1, inst.imm));
           state.regs.set(inst.rd, res);
         }},
        {OpType::SLL,
         [](const Instruction &inst, State &state) {
           auto rs1 = state.regs.get(inst.rs1);
           auto rs2 = state.regs.get(inst.rs2);
           state.regs.set(inst.rd, rs1 << rs2);
         }},
        {OpType::SRL,
         [](const Instruction &inst, State &state) {
           executeRegisterRegisterOp(inst, state, executeSR<RegVal>);
         }},
        {OpType::SRA,
         [](const Instruction &inst, State &state) {
           auto rs1 = signCast(state.regs.get(inst.rs1));
           auto rs2 = signCast(state.regs.get(inst.rs2));
           auto res = unsignedCast(executeSR(rs1, rs2));
           state.regs.set(inst.rd, res);
         }},
        {OpType::BEQ,
         [](const Instruction &inst, State &state) {
           executeCondBranch(inst, state, std::equal_to<RegVal>());
         }},
        {OpType::BNE,
         [](const Instruction &inst, State &state) {
           executeCondBranch(inst, state, std::not_equal_to<RegVal>());
         }},
        {OpType::BLT,
         [](const Instruction &inst, State &state) {
           executeCondBranch(inst, state, std::less<RegVal>());
         }},
        {OpType::BLTU,
         [](const Instruction &inst, State &state) {
           executeCondBranch(inst, state, std::greater<RegVal>());
         }},
        {OpType::BGEU,
         [](const Instruction &inst, State &state) {
           executeCondBranch(inst, state, std::greater_equal<RegVal>());
         }},
        {OpType::BGE,
         [](const Instruction &inst, State &state) {
           auto rs1 = signCast(state.regs.get(inst.rs1));
           auto rs2 = signCast(state.regs.get(inst.rs2));
           if (rs1 >= rs2) {
             state.branchIsTaken = true;
             state.npc = state.pc + inst.imm;
           }
         }},
        {OpType::XOR,
         [](const Instruction &inst, State &state) {
           executeRegisterRegisterOp(inst, state, std::bit_xor<RegVal>());
         }},
        {OpType::CSRRW,
         [](const Instruction &inst, State &state) {
           auto rs1 = state.regs.get(inst.rs1);
           auto csr = state.csregs.get(inst.csr);
           state.csregs.set(inst.csr, rs1);
           state.regs.set(inst.rd, csr);
         }},
        {OpType::CSRRS,
         [](const Instruction &inst, State &state) {
           auto rs1 = state.regs.get(inst.rs1);
           auto csr = state.csregs.get(inst.csr);
           state.csregs.set(inst.csr, rs1 | csr);
           state.regs.set(inst.rd, csr);
         }},
        {OpType::CSRRC,
         [](const Instruction &inst, State &state) {
           auto rs1 = state.regs.get(inst.rs1);
           auto csr = state.csregs.get(inst.csr);
           state.csregs.set(inst.csr, rs1 & (~csr));
           state.regs.set(inst.rd, csr);
         }},
        {OpType::CSRRWI,
         [](const Instruction &inst, State &state) {
           auto rs1 = inst.rs1;
           auto csr = state.csregs.get(inst.csr);
           state.regs.set(inst.rd, csr);
           state.csregs.set(inst.csr, getBits<4, 0>(rs1));
         }},
        {OpType::CSRRSI,
         [](const Instruction &inst, State &state) {
           auto rs1 = inst.rs1;
           auto csr = state.csregs.get(inst.csr);
           state.regs.set(inst.rd, csr);
           state.csregs.set(inst.csr, csr | getBits<4, 0>(rs1));
         }},
        {OpType::CSRRCI,
         [](const Instruction &inst, State &state) {
           auto rs1 = inst.rs1;
           auto csr = state.csregs.get(inst.csr);
           state.regs.set(inst.rd, csr);
           state.csregs.set(inst.csr, csr & (~getBits<4, 0>(rs1)));
         }},
    };

} // namespace sim
