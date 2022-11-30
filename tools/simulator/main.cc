#include <filesystem>
#include <map>
#include <stdexcept>

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>

#include <spdlog/spdlog.h>

#include "hart/hart.hh"

namespace fs = std::filesystem;
namespace lvl = spdlog::level;

int main(int argc, char **argv) try {
  CLI::App app{"Simulator"};

  lvl::level_enum loggingLevel{};
  std::map<std::string, lvl::level_enum> map{
      {"trace", lvl::trace}, {"debug", lvl::debug}, {"info", lvl::info},
      {"warn", lvl::warn},   {"err", lvl::err},     {"critical", lvl::critical},
      {"off", lvl::off}};

  app.add_option("-l,--log", loggingLevel, "Level settings")
      ->required()
      ->transform(CLI::CheckedTransformer(map, CLI::ignore_case));

  fs::path input{};
  app.add_option("input", input, "Executable file")->required();

  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError &e) {
    return app.exit(e);
  }

  spdlog::set_level(loggingLevel);

  // spdlog::trace("trace");
  // spdlog::debug("debug");
  // spdlog::info("info");
  // spdlog::warn("warn");
  // spdlog::error("err");
  // spdlog::critical("critical");

  sim::Hart hart{input};
  hart.run();

  return 0;
} catch (const std::exception &e) {
  spdlog::error(e.what());
  return 1;
}
