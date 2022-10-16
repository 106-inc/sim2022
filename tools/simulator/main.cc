#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>

#include <spdlog/spdlog.h>

int main(int argc, char **argv) {
  CLI::App app{"Simulator"};

  bool isInfo;
  app.add_flag("-i,--info", isInfo, "Turn on info logging level");

  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError &e) {
    return app.exit(e);
  }

  spdlog::set_level(isInfo ? spdlog::level::info : spdlog::level::err);

  spdlog::info("Hello with info level");
  spdlog::error("Hello with error level");
  return 0;
}
