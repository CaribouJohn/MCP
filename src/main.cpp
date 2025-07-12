
#include <chrono>

// MCP Server main entry point
//
// Logging:
//   - All server events, incoming requests, and outgoing responses are logged
//   to 'mcp_server.log' by default.
//   - The log file location can be changed with the MCP_LOG_FILE environment
//   variable or --log-file argument.
//   - To check what was sent/received over stdio, open 'mcp_server.log'.
//   - Example log entries:
//       [2025-07-12 14:23:02] [info] [STDIO IN] { ...JSON-RPC request... }
//       [2025-07-12 14:23:02] [info] [STDIO OUT] { ...JSON-RPC response... }
//   - All logs are flushed immediately for real-time debugging.
//
// See also: 'bridge_stdio.log' for raw stdio bridge logs.
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>

#include "json_rpc.h"
#include "mcp_logger.h"
#include "mcp_server.h"
#include "stdio_adapter.h"
// #include "tcp_server_adapter.h" // Removed TCP support
#include "transport_adapter.h"

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

    // Adapter selection logic
    // Only support stdio transport
    std::unique_ptr<ITransportAdapter> adapter;
    spdlog::info("Using StdioAdapter (stdio-only mode)");
    adapter = std::make_unique<StdioAdapter>();

    std::string message;
    int requestCount = 0;

    while (adapter->readMessage(message)) {
      requestCount++;
      spdlog::debug("Received request #" + std::to_string(requestCount) + ": " +
                    message);

      if (!message.empty()) {
        std::string response;
        server.processRequest(message, response);

        if (!response.empty()) {
          spdlog::debug("Sending response #" + std::to_string(requestCount) +
                        ": " + response);
          adapter->writeMessage(response);
        } else {
          spdlog::warn("Empty response generated for request #" +
                       std::to_string(requestCount));
        }
      } else {
        spdlog::debug("Received empty message, ignoring");
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
