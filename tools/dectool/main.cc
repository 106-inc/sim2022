#include <exception>
#include <filesystem>
#include <iomanip>
#include <iostream>

#include <CLI/CLI.hpp>

#include "common/common.hh"
#include "decoder/decoder.hh"
#include "elfloader/elfloader.hh"

sim::Word toBE(sim::Word num) {
  static_assert(sizeof(decltype(num)) == 4);
  auto byte0 = (num & 0x000000ffu) << 24u;
  auto byte1 = (num & 0x0000ff00u) << 8u;
  auto byte2 = (num & 0x00ff0000u) >> 8u;
  auto byte3 = (num & 0xff000000u) >> 24u;

  return byte0 | byte1 | byte2 | byte3;
}

int main(int argc, char *argv[]) try {
  namespace fs = std::filesystem;

  CLI::App app{"Tool to disassemble sim elf"};

  fs::path elf_file{};
  app.add_option("elf_file", elf_file, "Elf file to disassemble")
      ->required()
      ->check(CLI::ExistingFile);

  CLI11_PARSE(app, argc, argv);

  sim::ELFLoader elf_loader{elf_file};
  auto text_sec = elf_loader.getTextSection();

  auto *start = text_sec->get_data();
  for (std::size_t i = 0; i < text_sec->get_size(); i += sizeof(sim::Word)) {
    auto bin_word = *reinterpret_cast<const sim::Word *>(start + i);

    auto inst = sim::Decoder::decode(bin_word);
    std::cout << std::hex << std::setw(4) << std::setfill('0') << i << "  "
              << std::setw(8) << toBE(bin_word) << std::dec
              << " :: " << inst.str() << std::endl;
  }

} catch (std::exception &ex) {
  std::cerr << "Error: " << ex.what() << std::endl;
  return 1;
} catch (...) {
  std::cout << "Bad exception\n";
  return 1;
}
