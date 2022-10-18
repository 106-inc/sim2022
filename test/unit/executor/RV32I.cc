#include "test_header.hh"
#include "executor_test.hh"

TEST(execute, add)
{
    simulationState.regs.set(2, 42);
    sim::Instruction instr = {
        1, // rs1
        2, // rs2
        0,
        1, // rd
        0,
        0,
        sim::OpType::ADD,
        41 // imm
    };

    ASSERT_EQ(simulationState.regs.get(1), 0);
    executor.execute(instr, simulationState);
    ASSERT_EQ(simulationState.regs.get(1), 42);
}

TEST(execute, sub)
{
    simulationState.regs.set(1, 15);
    simulationState.regs.set(2, 5);
    sim::Instruction instr = {
        1, // rs1
        2, // rs2
        0,
        3, // rd
        0,
        0,
        sim::OpType::SUB,
        42 // imm
    };

    executor.execute(instr, simulationState);
    ASSERT_EQ(simulationState.regs.get(3), 10);
}

TEST(execute, lw)
{
    // TODO
}

TEST(execute, sw)
{
    // TODO
}

TEST(execute, jal)
{
    simulationState.pc = 0x0;
    sim::Instruction instr = {
        1, // rs1
        2, // rs2
        0,
        3, // rd
        0,
        0,
        sim::OpType::JAL,
        0xFF // imm
    };
    executor.execute(instr, simulationState);
    ASSERT_EQ(simulationState.npc, 0xFF);

    simulationState.pc = 0xFF;
    instr = {
        1, // rs1
        2, // rs2
        0,
        3, // rd
        0,
        0,
        sim::OpType::JAL,
        0xFFFFFFF1, // imm
    };
    executor.execute(instr, simulationState);
    ASSERT_EQ(simulationState.npc, 0xF0);
}

TEST(execute, jalr)
{
    simulationState.regs.set(1, 0xA0);
    sim::Instruction instr = {
        1, // rs1
        2, // rs2
        0,
        3, // rd
        0,
        0,
        sim::OpType::JALR,
        0x06 // imm
    };
    executor.execute(instr, simulationState);
    ASSERT_EQ(simulationState.npc, 0xA6);

    simulationState.regs.set(1, 0xA0);
    instr = {
        1, // rs1
        2, // rs2
        0,
        3, // rd
        0,
        0,
        sim::OpType::JALR,
        0x03 // imm
    };
    executor.execute(instr, simulationState);
    ASSERT_EQ(simulationState.npc, 0xA2);
}

#include "test_footer.hh"
