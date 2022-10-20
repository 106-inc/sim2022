#include <iostream>
#include <string>

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>
#include <elfio/elfio.hpp>
#include <spdlog/spdlog.h>

namespace elf = ELFIO;

int main(int argc, char **argv) {
  spdlog::set_level(spdlog::level::err);

  CLI::App app{"ElfDump"};
  auto *input = app.add_option("input", "Input file in elf format")->required();
  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError &e) {
    return app.exit(e);
  }

  elf::elfio reader{};
  if (const auto &fname = input->as<std::string>(); !reader.load(fname)) {
    spdlog::error("Failed while loading input file: {}", fname);
    return 1;
  }

  std::cout << "ELF file class    : "
            << ((reader.get_class() == elf::ELFCLASS32) ? "ELF32" : "ELF64")
            << std::endl;

  std::cout << "ELF file encoding : "
            << ((reader.get_encoding() == elf::ELFDATA2LSB) ? "Little endian"
                                                            : "Big endian")
            << std::endl;

  const auto &sections = reader.sections;
  std::cout << "Number of sections: " << sections.size() << std::endl;
  for (std::size_t i = 0; const auto *section : sections) {
    std::cout << "  [" << i++ << "] " << section->get_name() << "\t"
              << section->get_size() << std::endl;
  }

  const auto &segments = reader.segments;
  std::cout << "Number of segments: " << segments.size() << std::endl;
  for (std::size_t i = 0; const auto *segment : segments) {
    std::cout << "  [" << i++ << "] 0x" << std::hex << segment->get_flags()
              << "\t0x" << segment->get_virtual_address() << "\t0x"
              << segment->get_file_size() << "\t0x"
              << segment->get_memory_size() << std::endl;
  }

  return 0;
}
