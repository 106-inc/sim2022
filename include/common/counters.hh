#ifndef __INCLUDE_COMMON_COUNTERS_HH__
#define __INCLUDE_COMMON_COUNTERS_HH__

#include <unordered_map>

#include "common.hh"

namespace sim {

class Counters final {
public:
  /*
  the average number of core clock cycles per instruction for a series of inde-
  pendent instructions of the same kind in the same thread
  https://www.agner.org/optimize/instruction_tables.pdf
  */
  using Throughput = std::uint16_t;
  /* name bindings to CSR */
  enum CSRBindings {
    CYCLE = 0xC00,   /* Cycle counter for RDCYCLE instruction */
    TIME = 0xC01,    /* Timer for RDTIME instruction */
    INSTRET = 0xC02, /* Instructions-retired counter for RDINSTRET */

    CYCLEH = 0xC80,   /* Upper 32 bits of cycle, RV32 only */
    TIMEH = 0xC81,    /* Upper 32 bits of time, RV32 only */
    INSTRETH = 0xC82, /* Upper 32 bits of instret, RV32 only */
  };

  static Throughput getThroughput(OpType type) {
    static const std::unordered_map<OpType, Throughput> throughput = {
        {OpType::UNKNOWN, 0}, {OpType::ADD, 1},    {OpType::SUB, 1},
        {OpType::MUL, 3},     {OpType::DIV, 36},   {OpType::LW, 6},
        {OpType::SW, 6},      {OpType::JAL, 36},   {OpType::JALR, 36},
        {OpType::ECALL, 36},  {OpType::ADDI, 1},   {OpType::ANDI, 1},
        {OpType::XORI, 1},    {OpType::ORI, 1},    {OpType::SLTI, 2},
        {OpType::SLTIU, 1},   {OpType::LUI, 2},    {OpType::AUIPC, 3},
        {OpType::SLLI, 2},    {OpType::SRLI, 2},   {OpType::SRAI, 2},
        {OpType::SLL, 2},     {OpType::SRL, 2},    {OpType::SRA, 2},
        {OpType::BEQ, 36},    {OpType::BNE, 36},   {OpType::BLT, 36},
        {OpType::BLTU, 36},   {OpType::BGEU, 36},  {OpType::BGE, 36},
        {OpType::XOR, 1},     {OpType::CSRRW, 1},  {OpType::CSRRS, 1},
        {OpType::CSRRC, 1},   {OpType::CSRRWI, 1}, {OpType::CSRRSI, 1},
        {OpType::CSRRCI, 1},
    };
    auto iter = throughput.find(type);
    if (iter == throughput.end()) {
#ifdef SPDLOG
      spdlog::warn("Cant calculate {} throughput",
                   static_cast<std::uint8_t>(type));
#endif
      return 1;
    }
    return iter->second;
  }
}; // class Counters

} // namespace sim

#endif // __INCLUDE_COMMON_COUNTERS_HH__
