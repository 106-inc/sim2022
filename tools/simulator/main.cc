#include <chrono>
#include <filesystem>
#include <iostream>
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

void initCosimLogger(const fs::path &cosimFile, bool toStdout) {
  auto logger = toStdout ? spdlog::stdout_color_mt(sim::kCosimLoggerName.data())
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

  app.add_option("-l,--log", loggingLevel, "Level settings")
      ->transform(CLI::CheckedTransformer(map, CLI::ignore_case))
      ->default_val("warn");

  fs::path input{};
  app.add_option("input", input, "Executable file")
      ->required()
      ->check(CLI::ExistingFile);

  auto *isCosimOpt = app.add_flag("--cosim", "Enable cosim mode");

  fs::path cosimFile{};
  auto *cosimFileOpt = app.add_option("--cosim-file", cosimFile,
                                      "Dump cosim to file instead of stdout")
                           ->expected(0, 1)
                           ->default_val("./trace.txt")
                           ->check(!CLI::ExistingDirectory)
                           ->needs(isCosimOpt);

  isCosimOpt->check(
      [&loggingLevel, &cosimFileOpt](const std::string &) -> std::string {
        if (loggingLevel < lvl::warn && !*cosimFileOpt)
          return "To dump cosim data to stdout, at least warn log level is "
                 "required";
        return "";
      });

  bool printPerf{false};
  app.add_flag("--print-perf", printPerf,
               "Print information about performance");

  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError &e) {
    return app.exit(e);
  }

  spdlog::set_level(loggingLevel);
  if (*isCosimOpt) {
    initCosimLogger(cosimFile, !*cosimFileOpt);
  }
  sim::Hart hart{input};

  auto start = std::chrono::steady_clock::now();
  hart.run();
  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<double> elapsedSeconds = end - start;

  if (printPerf) {
    auto ic = hart.getInstrCount();
    auto tm = elapsedSeconds.count();
    std::cout << "Instruction number: " << ic << std::endl;
    std::cout << "Elapsed time: " << tm << "s" << std::endl;
    std::cout << "Perf: " << (ic / tm / 1e6) << "MIPS" << std::endl;
  }

  return 0;
} catch (const std::exception &e) {
  spdlog::error(e.what());
  return 1;
}
