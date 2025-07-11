#include "handlers/list_tools_handler.h"

#include "mcp_logger.h"

ListToolsHandler::ListToolsHandler(const McpServer& server, JsonRpc& rpc)
    : server_(server), jsonRpc_(rpc) {}

void ListToolsHandler::handle(const json& request, std::string& response) {
  std::string method, id;
  json params;
  if (jsonRpc_.parseRequest(request.dump(), method, params, id)) {
    json toolsArray = json::array();
    for (const auto& [name, tool] : server_.getTools()) {
      json toolObj = {{"name", tool.name},
                      {"description", tool.description},
                      {"inputSchema", tool.inputSchema}};
      toolsArray.push_back(toolObj);
    }
    json result = {{"tools", toolsArray}};
    response = jsonRpc_.createResponse(id, result);
  } else {
    response = jsonRpc_.createErrorResponse(id, -32700, "Parse error");
  }
}
