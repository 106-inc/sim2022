#include <filesystem>
#include <map>
#include <stdexcept>

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include "common/common.hh"
#include "hart/hart.hh"

namespace fs = std::filesystem;
namespace lvl = spdlog::level;

void initCosimLogger() {
  auto logger =
      spdlog::basic_logger_mt(sim::kCosimLoggerName.data(), "trace.txt", true);
  logger->set_pattern("%v");
  logger->set_level(spdlog::level::info);
}

int main(int argc, char **argv) try {
  CLI::App app{"Simulator"};

  lvl::level_enum loggingLevel{};
  std::map<std::string, lvl::level_enum> map{
      {"trace", lvl::trace}, {"debug", lvl::debug},       {"info", lvl::info},
      {"warn", lvl::warn},   {"warning", lvl::warn},      {"err", lvl::err},
      {"error", lvl::err},   {"critical", lvl::critical}, {"off", lvl::off}};

  app.add_option("-l,--log", loggingLevel, "Level settings")
      ->transform(CLI::CheckedTransformer(map, CLI::ignore_case))
      ->default_val("warn");

  fs::path input{};
  app.add_option("input", input, "Executable file")->required();

  bool isCosim{false};
  app.add_flag("--trace", isCosim, "dump execution trace for cosim");

  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError &e) {
    return app.exit(e);
  }

  spdlog::set_level(loggingLevel);
  if (isCosim) {
    initCosimLogger();
  }
  sim::Hart hart{input};
  hart.run();

  return 0;
} catch (const std::exception &e) {
  spdlog::error(e.what());
  return 1;
}
