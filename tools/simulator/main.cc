#include <filesystem>

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>

#include <spdlog/spdlog.h>

#include "hart/hart.hh"

namespace fs = std::filesystem;

int main(int argc, char **argv) {
  CLI::App app{"Simulator"};

  bool isInfo;
  app.add_flag("-i,--info", isInfo, "Turn on info logging level");

  // input
  fs::path input{};                                    // input
  app.add_option("input", input, "input")->required(); // input

  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError &e) {
    return app.exit(e);
  }

  sim::Hart hart{input};
  hart.run();

  return 0;
}
