#pragma once
#include <fstream>
#include <iostream>
#include <string>

#include "transport_adapter.h"
// TCP support removed: stdio only

// Adapter for stdin/stdout transport (single client, CLI mode)
//
// StdioAdapter bridges VS Code (or any client) to the MCP server over stdio.
// - When VS Code connects, a log entry is written indicating the connection.
// - Every message received from stdin is logged as [IN] with a timestamp.
// - Every message sent to stdout is logged as [OUT] with a timestamp.
// - All logs are written to 'bridge_stdio.log' in the working directory,
// flushed immediately.
//
// To check what was sent/received, open 'bridge_stdio.log'. Each entry shows
// the direction, timestamp, and message content.
//
// Example log entries:
//   2025-07-12 14:23:01 [CONNECT] VS Code stdio bridge established
//   2025-07-12 14:23:02 [IN] { ...JSON-RPC request... }
//   2025-07-12 14:23:02 [OUT] { ...JSON-RPC response... }
//
// This makes it easy to trace all stdio communication for debugging.
class StdioAdapter : public ITransportAdapter {
 public:
  StdioAdapter();
  ~StdioAdapter();
  bool readMessage(std::string& message) override;
  bool writeMessage(const std::string& message) override;

 private:
  std::ofstream logFile_;
  void log(const std::string& direction, const std::string& msg);
};
