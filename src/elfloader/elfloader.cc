#include <stdexcept>

#include "elfloader/elfloader.hh"

namespace sim {

ELFLoader::ELFLoader(const fs::path &file) {
  if (!elfFile_.load(file))
    throw std::runtime_error{"Failed while loading input file: " +
                             file.string()};
}

ELFLoader::ELFLoader(std::istream &stream) {
  if (!elfFile_.load(stream))
    throw std::runtime_error{"Failed while loading input stream"};
}

Addr ELFLoader::getEntryPoint() const {
  return static_cast<Addr>(elfFile_.get_entry());
}

std::span<const Word> ELFLoader::getSection(const std::string &name) const {
  auto *section = getSectionPtr(name);

  auto *data = reinterpret_cast<const Word *>(section->get_data());
  constexpr auto factor = sizeof(Word) / sizeof(char);

  return std::span<const Word>{data, section->get_size() / factor};
}

Addr ELFLoader::getSectionAddr(const std::string &name) const {
  auto *section = getSectionPtr(name);
  return static_cast<Addr>(section->get_address());
}

bool ELFLoader::hasSection(const std::string &name) const {
  return elfFile_.sections[name] != nullptr;
}

const ELFIO::section *ELFLoader::getSectionPtr(const std::string &name) const {
  auto *section = elfFile_.sections[name];

  if (section == nullptr)
    throw std::runtime_error{"Unknown section name: " + name};

  return section;
}

} // namespace sim
