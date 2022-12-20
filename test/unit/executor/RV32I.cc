#include "executor_test.hh"
#include "test_header.hh"

TEST(execute, ADD) {
  simulationState.regs.set(2, 42);
  sim::Instruction instr = {
      1, // rs1
      2, // rs2
      0,
      1, // rd
      0, 0, sim::OpType::ADD,
      41 // imm
  };

  ASSERT_EQ(simulationState.regs.get(1), 0);
  executor.execute(instr, simulationState);
  ASSERT_EQ(simulationState.regs.get(1), 42);
}

TEST(execute, SUB) {
  simulationState.regs.set(1, 15);
  simulationState.regs.set(2, 5);
  sim::Instruction instr = {
      1, // rs1
      2, // rs2
      0,
      3, // rd
      0, 0, sim::OpType::SUB,
      42 // imm
  };

  executor.execute(instr, simulationState);
  ASSERT_EQ(simulationState.regs.get(3), 10);
}

TEST(execute, lWAndSW) {
  simulationState.regs.set(1, 0xA0);
  simulationState.regs.set(2, 0xFF);
  sim::Instruction instr = {
      1, // rs1
      2, // rs2
      0,
      0, // rd
      0,  0, sim::OpType::SW,
      0x0 // imm
  };
  executor.execute(instr, simulationState);
  ASSERT_EQ(simulationState.mem.loadWord(0xA0), 0xFF);
  instr = {
      1, // rs1
      2, // rs2
      0,
      3, // rd
      0,  0, sim::OpType::LW,
      0x0 // imm
  };
  executor.execute(instr, simulationState);
  ASSERT_EQ(simulationState.regs.get(3), 0xFF);

  // negative offset
  simulationState.regs.set(1, 0xA0);
  simulationState.regs.set(2, 0xFF);
  instr = {
      1, // rs1
      2, // rs2
      0,
      0, // rd
      0,         0, sim::OpType::SW,
      0xFFFFFFF0 // imm(-16)
  };
  executor.execute(instr, simulationState);
  ASSERT_EQ(simulationState.mem.loadWord(0x90), 0xFF);
  instr = {
      1, // rs1
      2, // rs2
      0,
      3, // rd
      0,         0, sim::OpType::LW,
      0xFFFFFFF0 // imm
  };
  executor.execute(instr, simulationState);
  ASSERT_EQ(simulationState.regs.get(3), 0xFF);
}

TEST(execute, JAL) {
  simulationState.pc = 0x0;
  sim::Instruction instr = {
      1, // rs1
      2, // rs2
      0,
      3, // rd
      0,   0, sim::OpType::JAL,
      0xFF // imm
  };
  executor.execute(instr, simulationState);
  ASSERT_EQ(simulationState.npc, 0xFF);

  // negative offset
  simulationState.pc = 0xFF;
  instr = {
      1, // rs1
      2, // rs2
      0,
      3, // rd
      0,          0, sim::OpType::JAL,
      0xFFFFFFF1, // imm
  };
  executor.execute(instr, simulationState);
  ASSERT_EQ(simulationState.npc, 0xF0);
}

TEST(execute, JALR) {
  simulationState.regs.set(1, 0xA0);
  sim::Instruction instr = {
      1, // rs1
      2, // rs2
      0,
      3, // rd
      0,   0, sim::OpType::JALR,
      0x06 // imm
  };
  executor.execute(instr, simulationState);
  ASSERT_EQ(simulationState.npc, 0xA6);

  // setBit
  simulationState.regs.set(1, 0xA0);
  instr = {
      1, // rs1
      2, // rs2
      0,
      3, // rd
      0,   0, sim::OpType::JALR,
      0x03 // imm
  };
  executor.execute(instr, simulationState);
  ASSERT_EQ(simulationState.npc, 0xA2);

  // negative imm + setBit
  simulationState.regs.set(1, 0xA0);
  instr = {
      1, // rs1
      2, // rs2
      0,
      3, // rd
      0,          0, sim::OpType::JALR,
      0xFFFFFFFF, // imm
  };
  executor.execute(instr, simulationState);
  ASSERT_EQ(simulationState.npc, 0x9E);
}

TEST(execute, ADDI) {
    simulationState.regs.set(20, 0xA0);
    sim::Instruction instr = {
        20, // rs1
        15, // rs2
        0,
        11, // rd
        0,
        0,
        sim::OpType::ADDI,
        0x06 // imm
    };
    executor.execute(instr, simulationState);
    ASSERT_EQ(simulationState.regs.get(11), 0xA6);
}

TEST(execute, SLTI) {
    // basic compare with result 0
    simulationState.regs.set(20, 0xA0);
    simulationState.regs.set(11, 0xA0);
    sim::Instruction instr = {
        20, // rs1
        15, // rs2
        0,
        11, // rd
        0,
        0,
        sim::OpType::SLTI,
        0x06 // imm
    };
    executor.execute(instr, simulationState);
    ASSERT_EQ(simulationState.regs.get(11), 0x0);

    // basic compare with result 1
    instr = {
        20, // rs1
        15, // rs2
        0,
        11, // rd
        0,
        0,
        sim::OpType::SLTI,
        0xAA6 // imm
    };
    executor.execute(instr, simulationState);
    ASSERT_EQ(simulationState.regs.get(11), 0x1);

    // compare signed negative nums with result 0
    simulationState.regs.set(20, 0xFFFFFFFF);
    simulationState.regs.set(11, 0xFFFFFFFE);
    instr = {
        20, // rs1
        15, // rs2
        0,
        11, // rd
        0,
        0,
        sim::OpType::SLTI,
        0xFFFFFFFE // imm
    };
    executor.execute(instr, simulationState);
    ASSERT_EQ(simulationState.regs.get(11), 0x0);
}

TEST(execute, SLTIU) {
    // compare unsigned nums with result 0
    simulationState.regs.set(20, 0xFFFFFFFF);
    simulationState.regs.set(11, 0xFFFFFFFE);
    sim::Instruction instr = {
        20, // rs1
        15, // rs2
        0,
        11, // rd
        0,
        0,
        sim::OpType::SLTIU,
        0xFFFFFFFE // imm
    };
    executor.execute(instr, simulationState);
    ASSERT_EQ(simulationState.regs.get(11), 0x0);
}

TEST(execute, ORI) {
    simulationState.regs.set(20, 0x0F0F0F0F);
    sim::Instruction instr = {
        20, // rs1
        15, // rs2
        0,
        11, // rd
        0,
        0,
        sim::OpType::ORI,
        0xF0F0F0F0 // imm
    };
    executor.execute(instr, simulationState);
    ASSERT_EQ(simulationState.regs.get(11), 0xFFFFFFFF);
}

TEST(execute, XORI) {
    simulationState.regs.set(20, 0x0F0F0F0F);
    sim::Instruction instr = {
        20, // rs1
        15, // rs2
        0,
        11, // rd
        0,
        0,
        sim::OpType::XORI,
        0x00F0F0F0 // imm
    };
    executor.execute(instr, simulationState);
    ASSERT_EQ(simulationState.regs.get(11), 0x0FFFFFFF);
}

TEST(execute, LUI) {
    sim::Instruction instr = {
        20, // rs1
        15, // rs2
        0,
        11, // rd
        0,
        0,
        sim::OpType::LUI,
        0x1 // imm
    };
    executor.execute(instr, simulationState);
    ASSERT_EQ(simulationState.regs.get(11), 0x1000);
}

TEST(execute, AUIPC) {
    simulationState.pc = 0x1;
    sim::Instruction instr = {
        20, // rs1
        15, // rs2
        0,
        11, // rd
        0,
        0,
        sim::OpType::AUIPC,
        0x1 // imm
    };
    executor.execute(instr, simulationState);
    ASSERT_EQ(simulationState.regs.get(11), 0x1001);
}

TEST(execute, SLLI) {
    simulationState.regs.set(20, 0x1);
    sim::Instruction instr = {
        20, // rs1
        15, // rs2
        0,
        11, // rd
        0,
        0,
        sim::OpType::SLLI,
        0xDEADBEEF // imm
    };
    executor.execute(instr, simulationState);
    ASSERT_EQ(simulationState.regs.get(11), 0x8000);
}

TEST(execute, SRLI) {
    simulationState.regs.set(20, 0xFFFFFFFF);
    sim::Instruction instr = {
        20, // rs1
        15, // rs2
        0,
        11, // rd
        0,
        0,
        sim::OpType::SRLI,
        0x4 // imm
    };
    executor.execute(instr, simulationState);
    ASSERT_EQ(simulationState.regs.get(11), 0x0FFFFFFF);
}

TEST(execute, SRAI) {
    simulationState.regs.set(20, 0xFFFFFFFF);
    sim::Instruction instr = {
        20, // rs1
        15, // rs2
        0,
        11, // rd
        0,
        0,
        sim::OpType::SRAI,
        0x4 // imm
    };
    executor.execute(instr, simulationState);
    ASSERT_EQ(simulationState.regs.get(11), 0xFFFFFFFF);
}

TEST(execute, SLL) {
    simulationState.regs.set(20, 0x1);
    simulationState.regs.set(15, 0x4);
    sim::Instruction instr = {
        20, // rs1
        15, // rs2
        0,
        11, // rd
        0,
        0,
        sim::OpType::SLL,
        0x12 // imm
    };
    executor.execute(instr, simulationState);
    ASSERT_EQ(simulationState.regs.get(11), 0x10);
}

TEST(execute, SRL) {
    simulationState.regs.set(20, 0xFFFFFFFF);
    simulationState.regs.set(15, 0x4);
    sim::Instruction instr = {
        20, // rs1
        15, // rs2
        0,
        11, // rd
        0,
        0,
        sim::OpType::SRL,
        0x12 // imm
    };
    executor.execute(instr, simulationState);
    ASSERT_EQ(simulationState.regs.get(11), 0x0FFFFFFF);
}

TEST(execute, SRA) {
    simulationState.regs.set(20, 0xFFFFFFFF);
    simulationState.regs.set(15, 0x4);
    sim::Instruction instr = {
        20, // rs1
        15, // rs2
        0,
        11, // rd
        0,
        0,
        sim::OpType::SRA,
        0x12 // imm
    };
    executor.execute(instr, simulationState);
    ASSERT_EQ(simulationState.regs.get(11), 0xFFFFFFFF);
}

TEST(execute, CondBranches) {
    simulationState.regs.set(20, 0xFFFFFFFF);
    simulationState.regs.set(15, 0xFFFFFFFF);
    simulationState.pc = 0x1;
    sim::Instruction instr = {
        20, // rs1
        15, // rs2
        0,
        11, // rd
        0,
        0,
        sim::OpType::BEQ,
        0x12 // imm
    };
    executor.execute(instr, simulationState);
    ASSERT_EQ(simulationState.npc, 0x13);

    instr = {
        20, // rs1

        15, // rs2
        0,
        11, // rd
        0,
        0,
        sim::OpType::BNE,
        0x14 // imm
    };
    executor.execute(instr, simulationState);
    ASSERT_EQ(simulationState.npc, 0x13);
}

#include "test_footer.hh"
