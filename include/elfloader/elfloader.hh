#ifndef __INCLUDE_ELFLOADER_ELFLOADER_HH__
#define __INCLUDE_ELFLOADER_ELFLOADER_HH__

#include <filesystem>

#include <elfio/elfio.hpp>

#include "common/common.hh"

namespace sim {

namespace fs = std::filesystem;

class ELFLoader final {
private:
  ELFIO::elfio elfFile_{};

public:
  ELFLoader(fs::path elfPath);

  Addr getEntryPoint() const;
};

} // namespace sim

#endif // __INCLUDE_ELFLOADER_ELFLOADER_HH__
