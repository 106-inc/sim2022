#ifndef __INCLUDE_ELFLOADER_ELFLOADER_HH__
#define __INCLUDE_ELFLOADER_ELFLOADER_HH__

#include <filesystem>
#include <span>

#include <elfio/elfio.hpp>

#include "common/common.hh"

namespace sim {

namespace fs = std::filesystem;

class ELFLoader final {
private:
  ELFIO::elfio elfFile_{};

public:
  ELFLoader(const fs::path &file);
  ELFLoader(std::istream &stream);

  Addr getEntryPoint() const;

  std::span<const Word> getSection(const std::string &name) const;
  Addr getSectionAddr(const std::string &name) const;
  bool hasSection(const std::string &name) const;

  using IndexT = unsigned;
  std::vector<IndexT> getLoadableSegments() const;

  std::span<const Word> getSegment(IndexT index) const;
  Addr getSegmentAddr(IndexT index) const;
  bool hasSegment(IndexT index) const;

private:
  void check() const;
  const ELFIO::section *getSectionPtr(const std::string &name) const;
  const ELFIO::segment *getSegmentPtr(IndexT index) const;
};

} // namespace sim

#endif // __INCLUDE_ELFLOADER_ELFLOADER_HH__
