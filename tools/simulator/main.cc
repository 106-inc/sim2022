#include <filesystem>
#include <map>
#include <stdexcept>

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "common/common.hh"
#include "hart/hart.hh"

namespace fs = std::filesystem;
namespace lvl = spdlog::level;

void initCosimLogger(const fs::path &cosimFile) {
  auto logger = cosimFile.empty()
                    ? spdlog::stdout_color_mt(sim::kCosimLoggerName.data())
                    : spdlog::basic_logger_mt(sim::kCosimLoggerName.data(),
                                              cosimFile, true);
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

  auto *logOpt = app.add_option("-l,--log", loggingLevel, "Level settings")
                     ->transform(CLI::CheckedTransformer(map, CLI::ignore_case))
                     ->default_val("warn");

  fs::path input{};
  app.add_option("input", input, "Executable file")->required();

  fs::path cosimFile{};
  auto *cosimOpt =
      app.add_option("--cosim", cosimFile,
                     "Cosimulation mode. Dump to specified file or to stdout")
          ->expected(0, 1)
          ->excludes(logOpt);

  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError &e) {
    return app.exit(e);
  }

  spdlog::set_level(loggingLevel);
  if (*cosimOpt) {
    initCosimLogger(cosimFile);
  }
  sim::Hart hart{input};
  hart.run();

  return 0;
} catch (const std::exception &e) {
  spdlog::error(e.what());
  return 1;
}
