#include "handlers/call_tool_handler.h"

#include "mcp_logger.h"

CallToolHandler::CallToolHandler(const McpServer& server, JsonRpc& rpc)
    : server_(server), jsonRpc_(rpc) {}

void CallToolHandler::handle(const json& request, std::string& response) {
  spdlog::info("Handling tools/call request");
  std::string method, id;
  json params;
  if (jsonRpc_.parseRequest(request.dump(), method, params, id)) {
    std::string toolName = params["name"];
    json arguments =
        params.contains("arguments") ? params["arguments"] : json::object();
    spdlog::info("Tool call request - name: " + toolName +
                 ", arguments: " + arguments.dump());
    // Use the tool handler if it exists
    auto& handlers = server_.getToolHandlers();
    if (handlers.find(toolName) != handlers.end()) {
      try {
        spdlog::info("Calling tool: " + toolName);
        json result = handlers.at(toolName)(arguments);
        json contentArray = json::array();
        json textContent = {
            {"type", "text"},
            {"text",
             result.is_string() ? result.get<std::string>() : result.dump()}};
        contentArray.push_back(textContent);
        json resultObj = {{"content", contentArray}};
        response = jsonRpc_.createResponse(id, resultObj);
        spdlog::info("Tool call completed successfully: " + toolName);
      } catch (const std::exception& e) {
        spdlog::error("Tool call failed: " + toolName + " - " + e.what());
        response = jsonRpc_.createErrorResponse(id, -32603, e.what());
      }
    } else {
      spdlog::error("Tool not found: " + toolName);
      response = jsonRpc_.createErrorResponse(id, -32601,
                                              "Tool not found: " + toolName);
    }
  } else {
    spdlog::error("Failed to parse tools/call request");
    response = jsonRpc_.createErrorResponse(id, -32700, "Parse error");
  }
}
