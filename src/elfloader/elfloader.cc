#include <stdexcept>

#include "elfloader/elfloader.hh"

namespace sim {

ELFLoader::ELFLoader(fs::path elfPath) {
  if (!elfFile_.load(elfPath))
    throw std::runtime_error{"Failed while loading input file: " +
                             elfPath.string()};
}

Addr ELFLoader::getEntryPoint() const {
  return static_cast<Addr>(elfFile_.get_entry());
}

} // namespace sim
