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
  explicit ELFLoader(const fs::path &file);
  explicit ELFLoader(std::istream &stream);

  [[nodiscard]] Addr getEntryPoint() const;

  using IndexT = unsigned;
  [[nodiscard]] std::vector<IndexT> getLoadableSegments() const;

  [[nodiscard]] std::size_t getSegmentFileSize(IndexT index) const;
  [[nodiscard]] std::size_t getSegmentMemorySize(IndexT index) const;
  [[nodiscard]] std::span<const Word> getSegment(IndexT index) const;
  [[nodiscard]] Addr getSegmentAddr(IndexT index) const;
  [[nodiscard]] bool hasSegment(IndexT index) const;

private:
  void check() const;
  [[nodiscard]] const ELFIO::section *
  getSectionPtr(const std::string &name) const;
  [[nodiscard]] const ELFIO::segment *getSegmentPtr(IndexT index) const;
};

} // namespace sim

#endif // __INCLUDE_ELFLOADER_ELFLOADER_HH__
