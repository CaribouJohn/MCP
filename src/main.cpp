
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>

#include "json_rpc.h"
#include "mcp_logger.h"
#include "mcp_server.h"

int main(int argc, char* argv[]) {
  try {
    // Disable buffering on stdout for immediate response delivery
    std::cout.setf(std::ios::unitbuf);

    // --- Logging setup ---
    std::string log_level_str = "info";
    std::string log_file = "C:/Development/MCP/mcp_server.log";
    bool also_console = true;

    // Allow log level and file to be set via environment or args
    if (const char* env_log = std::getenv("MCP_LOG_LEVEL")) {
      log_level_str = env_log;
    }
    if (const char* env_file = std::getenv("MCP_LOG_FILE")) {
      log_file = env_file;
    }
    for (int i = 1; i < argc; ++i) {
      std::string arg = argv[i];
      if (arg == "--log-level" && i + 1 < argc) {
        log_level_str = argv[++i];
      } else if (arg == "--log-file" && i + 1 < argc) {
        log_file = argv[++i];
      } else if (arg == "--no-console-log") {
        also_console = false;
      }
    }

    spdlog::level::level_enum log_level = spdlog::level::info;
    if (log_level_str == "debug")
      log_level = spdlog::level::debug;
    else if (log_level_str == "warn")
      log_level = spdlog::level::warn;
    else if (log_level_str == "error")
      log_level = spdlog::level::err;
    else if (log_level_str == "trace")
      log_level = spdlog::level::trace;
    else if (log_level_str == "critical")
      log_level = spdlog::level::critical;
    else if (log_level_str == "off")
      log_level = spdlog::level::off;

    McpLogging::setup_logger("mcp_server", log_level, log_file, also_console);

    spdlog::info("=== MCP Server Starting ===");

    // Create MCP server instance
    McpServer server("cpp-mcp-server", "1.0.0");

    // Initialize server
    server.initialize();

    spdlog::info("Server initialized, starting main communication loop");

    // For MCP, we need to communicate via stdin/stdout
    // This is how VS Code will communicate with the server
    std::string line;
    int requestCount = 0;

    while (std::getline(std::cin, line)) {
      requestCount++;
      spdlog::debug("Received request #" + std::to_string(requestCount) + ": " +
                    line);

      if (!line.empty()) {
        std::string response;
        server.processRequest(line, response);

        if (!response.empty()) {
          spdlog::debug("Sending response #" + std::to_string(requestCount) +
                        ": " + response);
          std::cout << response << std::endl;
          std::cout.flush();
        } else {
          spdlog::warn("Empty response generated for request #" +
                       std::to_string(requestCount));
        }
      } else {
        spdlog::debug("Received empty line, ignoring");
      }
    }

    spdlog::info("Main loop ended - stdin closed");
  } catch (const std::exception& e) {
    spdlog::error(std::string("Exception in main: ") + e.what());
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
