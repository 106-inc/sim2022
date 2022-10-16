#include <spdlog/spdlog.h>

int main() {
  spdlog::set_level(spdlog::level::info);
  spdlog::info("Hello with info level");
  spdlog::error("Hello with error level");
  return 0;
}
