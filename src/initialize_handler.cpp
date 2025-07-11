#include "handlers/initialize_handler.h"

#include "mcp_logger.h"

InitializeHandler::InitializeHandler(const McpServer& server, JsonRpc& rpc)
    : server_(server), jsonRpc_(rpc) {}

void InitializeHandler::handle(const json& request, std::string& response) {
  std::string method, id;
  json params;
  if (jsonRpc_.parseRequest(request.dump(), method, params, id)) {
    json result = {{"protocolVersion", "2024-11-05"},
                   {"serverInfo",
                    {{"name", server_.serverInfo_.name},
                     {"version", server_.serverInfo_.version}}},
                   {"capabilities",
                    {{"tools", server_.serverInfo_.capabilities.tools},
                     {"logging", server_.serverInfo_.capabilities.logging}}}};
    response = jsonRpc_.createResponse(id, result);
    spdlog::info("Initialize response created successfully");
  } else {
    spdlog::error("Failed to parse initialize request");
    response = jsonRpc_.createErrorResponse(id, -32700, "Parse error");
  }
}
