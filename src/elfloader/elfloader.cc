#include <stdexcept>

#include "common/common.hh"
#include "elfloader/elfloader.hh"

namespace sim {

ELFLoader::ELFLoader(const fs::path &file) {
  if (!elfFile_.load(file))
    throw std::runtime_error{"Failed while loading input file: " +
                             file.string()};

  check();
}

ELFLoader::ELFLoader(std::istream &stream) {
  if (!elfFile_.load(stream))
    throw std::runtime_error{"Failed while loading input stream"};

  check();
}

Addr ELFLoader::getEntryPoint() const {
  return static_cast<Addr>(elfFile_.get_entry());
}

std::vector<Word> ELFLoader::getSection(const std::string &name) const {
  auto *section = getSectionPtr(name);

  auto *data = reinterpret_cast<const Word *>(section->get_data());
  constexpr auto factor = sizeof(Word) / sizeof(char);

  return std::vector<Word>{data, data + section->get_size() / factor};
}

Addr ELFLoader::getSectionAddr(const std::string &name) const {
  auto *section = getSectionPtr(name);
  return static_cast<Addr>(section->get_address());
}

bool ELFLoader::hasSection(const std::string &name) const {
  return elfFile_.sections[name] != nullptr;
}

std::vector<ELFLoader::IndexT> ELFLoader::getLoadableSegments() const {
  std::vector<IndexT> res{};
  for (auto &&segment : elfFile_.segments)
    if (ELFIO::PT_LOAD == segment->get_type())
      res.push_back(segment->get_index());
  return res;
}

std::vector<Word> ELFLoader::getSegment(IndexT index) const {
  auto *segment = getSegmentPtr(index);

  auto *data = reinterpret_cast<const Word *>(segment->get_data());
  constexpr auto factor = sizeof(Word) / sizeof(char);

  auto segmentSize = segment->get_memory_size() / factor;

  if (data == nullptr)
    return std::vector<Word>(segmentSize);

  return std::vector<Word>{data, data + segmentSize};
}

Addr ELFLoader::getSegmentAddr(IndexT index) const {
  auto *segment = getSegmentPtr(index);
  return static_cast<Addr>(segment->get_virtual_address());
}

bool ELFLoader::hasSegment(IndexT index) const {
  return elfFile_.segments[index] != nullptr;
}

void ELFLoader::check() const {
  if (auto diagnosis = elfFile_.validate(); !diagnosis.empty())
    throw std::runtime_error{diagnosis};

  if (elfFile_.get_class() != ELFIO::ELFCLASS32)
    throw std::runtime_error{"Wrong elf file class: only elf32 supported"};

  if (elfFile_.get_encoding() != ELFIO::ELFDATA2LSB)
    throw std::runtime_error{
        "Wrong encoding: only 2's complement little endian supported"};

  if (elfFile_.get_type() != ELFIO::ET_EXEC)
    throw std::runtime_error{
        "Wrong file type: only executable files are supported"};

  if (elfFile_.get_machine() != ELFIO::EM_RISCV)
    throw std::runtime_error{"Wrong machine type: only RISC-V supported"};
}

const ELFIO::section *ELFLoader::getSectionPtr(const std::string &name) const {
  auto *section = elfFile_.sections[name];

  if (section == nullptr)
    throw std::runtime_error{"Unknown section name: " + name};

  return section;
}

const ELFIO::segment *ELFLoader::getSegmentPtr(IndexT index) const {
  auto *segment = elfFile_.segments[index];

  if (segment == nullptr)
    throw std::runtime_error{"Unknown segment index: " + std::to_string(index)};

  return segment;
}

} // namespace sim
