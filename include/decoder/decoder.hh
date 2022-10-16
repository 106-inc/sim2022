#ifndef __INCLUDE_DECODER_DECODER_HH__
#define __INCLUDE_DECODER_DECODER_HH__

#include "common/common.hh"
#include "common/inst.hh"

namespace sim {

class Decoder final {
public:
  Instruction decode(Word binInst);
};

} // namespace sim

#endif // __INCLUDE_DECODER_DECODER_HH__
