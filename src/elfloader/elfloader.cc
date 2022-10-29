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

std::span<const Word> ELFLoader::getSection(const std::string &name) const {
  auto *section = elfFile_.sections[name];

  auto *data = reinterpret_cast<const Word *>(section->get_data());
  constexpr auto factor = sizeof(Word) / sizeof(char);

  return std::span<const Word>{data, section->get_size() / factor};
}

} // namespace sim
