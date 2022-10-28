#ifndef __INCLUDE_DECODER_DECODER_HH__
#define __INCLUDE_DECODER_DECODER_HH__

#include <type_traits>

#include "common/common.hh"
#include "common/inst.hh"

namespace sim {

class Decoder final {
public:
  /**
   * @brief Decode an instruction function
   *
   * @param binInst instruction bytes to decode
   * @return Instruction decoded instruction
   */
  static Instruction decode(Word binInst);
};

} // namespace sim

#endif // __INCLUDE_DECODER_DECODER_HH__
