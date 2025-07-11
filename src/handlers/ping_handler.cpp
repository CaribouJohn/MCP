#include "handlers/ping_handler.h"

#include "mcp_logger.h"

PingHandler::PingHandler(const McpServer& server, JsonRpc& rpc)
    : server_(server), jsonRpc_(rpc) {}

void PingHandler::handle(const json& request, std::string& response) {
  std::string method, id;
  json params;
  if (jsonRpc_.parseRequest(request.dump(), method, params, id)) {
    json result = {{"status", "pong"}};
    response = jsonRpc_.createResponse(id, result);
  } else {
    response = jsonRpc_.createErrorResponse(id, -32700, "Parse error");
  }
}
