#pragma once
#include "json_rpc.h"
#include "mcp_request_handler.h"
#include "mcp_server.h"

// Handler for 'initialize' requests
class InitializeHandler : public McpRequestHandler {
 public:
  InitializeHandler(const McpServer& server, JsonRpc& rpc);
  void handle(const json& request, std::string& response) override;

 private:
  const McpServer& server_;
  JsonRpc& jsonRpc_;
};
