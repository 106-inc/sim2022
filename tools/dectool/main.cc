#include <exception>
#include <filesystem>
#include <iostream>

#include <CLI/CLI.hpp>

#include "common/common.hh"
#include "decoder/decoder.hh"
#include "elfloader/elfloader.hh"

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
  auto offset = text_sec->get_offset();
  for (std::size_t i = 0; i < text_sec->get_size(); i += sizeof(sim::Word)) {
    auto bin_word = *reinterpret_cast<const sim::Word *>(start + i);

    auto inst = sim::Decoder::decode(bin_word);
    std::cout << "0x" << std::hex << offset + i << std::dec
              << " :: " << inst.str() << std::endl;
  }

} catch (std::exception &ex) {
  std::cerr << "Error: " << ex.what() << std::endl;
  return 1;
} catch (...) {
  std::cout << "Bad exception\n";
  return 1;
}
