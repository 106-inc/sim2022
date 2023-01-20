#include "executor_test.hh"
#include "test_header.hh"

#include "common/counters.hh"

TEST(execute, timersUpdate) {
  ASSERT_EQ(0, simulationState.csregs.get(sim::Counters::CSRBindings::INSTRET));
  ASSERT_EQ(0, simulationState.csregs.get(sim::Counters::CSRBindings::CYCLE));
  simulationState.csregs.updateTimers(sim::OpType::JAL);
  ASSERT_EQ(1, simulationState.csregs.get(sim::Counters::CSRBindings::INSTRET));
  ASSERT_EQ(36, simulationState.csregs.get(sim::Counters::CSRBindings::CYCLE));

  ASSERT_EQ(0,
            simulationState.csregs.get(sim::Counters::CSRBindings::INSTRETH));
  simulationState.csregs.set(sim::Counters::CSRBindings::INSTRET,
                             std::numeric_limits<sim::Word>::max());
  simulationState.csregs.updateTimers(sim::OpType::JAL);
  ASSERT_EQ(0, simulationState.csregs.get(sim::Counters::CSRBindings::INSTRET));
  ASSERT_EQ(1,
            simulationState.csregs.get(sim::Counters::CSRBindings::INSTRETH));
}

TEST(execute, CSRRW) {
  simulationState.regs.set(20, 0xFFFFFFFF);
  simulationState.csregs.set(15, 0x4);
  sim::Instruction instr = {20, // rs1
                            30, // rs2
                            0,
                            11, // rd
                            0,    15,    sim::OpType::CSRRW,
                            0x12, false, sim::executeCSRRW};
  executor.execute(instr, simulationState);
  ASSERT_EQ(simulationState.regs.get(11), 0x4);
  ASSERT_EQ(simulationState.csregs.get(15), 0xFFFFFFFF);
}

TEST(execute, CSRRS) {
  simulationState.regs.set(20, 0xF0F0F0F0);
  simulationState.csregs.set(15, 0x0F0F0F0F);
  sim::Instruction instr = {20, // rs1
                            30, // rs2
                            0,
                            11, // rd
                            0,    15,    sim::OpType::CSRRS,
                            0x12, false, sim::executeCSRRS};
  executor.execute(instr, simulationState);
  ASSERT_EQ(simulationState.regs.get(11), 0x0F0F0F0F);
  ASSERT_EQ(simulationState.csregs.get(15), 0xFFFFFFFF);
}

TEST(execute, CSRRC) {
  simulationState.regs.set(20, 0x00000001);
  simulationState.csregs.set(15, 0xFFFFFFF0);
  sim::Instruction instr = {20, // rs1
                            30, // rs2
                            0,
                            11, // rd
                            0,    15,    sim::OpType::CSRRC,
                            0x12, false, sim::executeCSRRC};
  executor.execute(instr, simulationState);
  ASSERT_EQ(simulationState.regs.get(11), 0xFFFFFFF0);
  ASSERT_EQ(simulationState.csregs.get(15), 0x00000001);
}

TEST(execute, CSRRWI) {
  simulationState.csregs.set(15, 0xFFFFFFFF);
  sim::Instruction instr = {0x000000FF, // rs1
                            30,         // rs2
                            0,
                            11, // rd
                            0,          15,    sim::OpType::CSRRWI,
                            0x12,       false, sim::executeCSRRWI};
  executor.execute(instr, simulationState);
  ASSERT_EQ(simulationState.regs.get(11), 0xFFFFFFFF);
  ASSERT_EQ(simulationState.csregs.get(15), 0x0000001F);
}

TEST(execute, CSRRSI) {
  simulationState.csregs.set(15, 0xFFFFFFF0);
  sim::Instruction instr = {0x000000F0, // rs1
                            30,         // rs2
                            0,
                            11, // rd
                            0,          15,    sim::OpType::CSRRSI,
                            0x12,       false, sim::executeCSRRSI};
  executor.execute(instr, simulationState);
  ASSERT_EQ(simulationState.regs.get(11), 0xFFFFFFF0);
  ASSERT_EQ(simulationState.csregs.get(15), 0xFFFFFFF0);
}

TEST(execute, CSRRCI) {
  simulationState.csregs.set(15, 0xF0FFFFFF);
  sim::Instruction instr = {0x00000001, // rs1
                            30,         // rs2
                            0,
                            11, // rd
                            0,          15,    sim::OpType::CSRRCI,
                            0x12,       false, sim::executeCSRRCI};
  executor.execute(instr, simulationState);
  ASSERT_EQ(simulationState.regs.get(11), 0xF0FFFFFF);
  ASSERT_EQ(simulationState.csregs.get(15), 0xF0FFFFFE);
}

#include "test_footer.hh"
