#include "executor_test.hh"
#include "test_header.hh"

TEST(execute, mul) {
  simulationState.regs.set(2, 21);
  simulationState.regs.set(1, 2);
  sim::Instruction instr = {1, // rs1
                            2, // rs2
                            0,
                            3, // rd
                            0,
                            0,
                            sim::OpType::MUL,
                            41, // imm
                            false,
                            sim::executeMUL};
  executor.execute(instr, simulationState);
  ASSERT_EQ(simulationState.regs.get(3), 42);
}

TEST(execute, div) {
  simulationState.regs.set(2, 3);
  simulationState.regs.set(1, 15);
  sim::Instruction instr = {1, // rs1
                            2, // rs2
                            0,
                            3, // rd
                            0,
                            0,
                            sim::OpType::DIV,
                            41, // imm
                            false,
                            sim::executeDIV};
  executor.execute(instr, simulationState);
  EXPECT_NEAR(simulationState.regs.get(3), 5, 0.01);
}

#include "test_footer.hh"
