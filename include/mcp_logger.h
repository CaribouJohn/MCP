
#pragma once
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <string>
#include <vector>

// Logging setup utility for MCP server
namespace McpLogging {

inline std::shared_ptr<spdlog::logger> setup_logger(
    const std::string& name = "mcp_server",
    spdlog::level::level_enum level = spdlog::level::info,
    const std::string& logfile = "", bool also_console = true) {
  std::vector<spdlog::sink_ptr> sinks;
  if (!logfile.empty()) {
    sinks.push_back(
        std::make_shared<spdlog::sinks::basic_file_sink_mt>(logfile, true));
  }
  if (also_console) {
    sinks.push_back(std::make_shared<spdlog::sinks::stderr_color_sink_mt>());
  }
  auto logger =
      std::make_shared<spdlog::logger>(name, sinks.begin(), sinks.end());
  logger->set_level(level);
  logger->set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");
  spdlog::set_default_logger(logger);
  return logger;
}

}  // namespace McpLogging
