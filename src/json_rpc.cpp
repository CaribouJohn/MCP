#include "json_rpc.h"
#include <sstream>
#include <algorithm>

JsonRpc::JsonRpc()
{
}

JsonRpc::~JsonRpc()
{
}

bool JsonRpc::parseRequest(const std::string &jsonStr, std::string &method,
                           json &params, std::string &id)
{
    try
    {
        json j = json::parse(jsonStr);

        if (j.contains("method"))
            method = j["method"];

        if (j.contains("id"))
        {
            if (j["id"].is_string())
                id = j["id"];
            else if (j["id"].is_number())
                id = std::to_string(j["id"].get<int>());
        }

        if (j.contains("params"))
            params = j["params"];
        else
            params = json::object();

        return !method.empty();
    }
    catch (const json::exception &e)
    {
        return false;
    }
}

std::string JsonRpc::createResponse(const std::string &id, const json &result)
{
    json response = {
        {"jsonrpc", "2.0"},
        {"result", result}};

    // Preserve the original ID type - try to convert back to number if it was numeric
    try
    {
        int numericId = std::stoi(id);
        response["id"] = numericId;
    }
    catch (...)
    {
        response["id"] = id; // Keep as string if conversion fails
    }

    return response.dump(); // Single line for MCP compatibility
}

std::string JsonRpc::createErrorResponse(const std::string &id, int errorCode,
                                         const std::string &errorMessage)
{
    json response = {
        {"jsonrpc", "2.0"},
        {"error", {{"code", errorCode}, {"message", errorMessage}}}};

    // Preserve the original ID type - try to convert back to number if it was numeric
    try
    {
        int numericId = std::stoi(id);
        response["id"] = numericId;
    }
    catch (...)
    {
        response["id"] = id; // Keep as string if conversion fails
    }

    return response.dump(); // Single line for MCP compatibility
}

std::string JsonRpc::createNotification(const std::string &method,
                                        const json &params)
{
    json notification = {
        {"jsonrpc", "2.0"},
        {"method", method},
        {"params", params}};
    return notification.dump(); // Single line for MCP compatibility
}

std::string JsonRpc::escapeJson(const std::string &str)
{
    std::string escaped;
    for (char c : str)
    {
        switch (c)
        {
        case '"':
            escaped += "\\\"";
            break;
        case '\\':
            escaped += "\\\\";
            break;
        case '\b':
            escaped += "\\b";
            break;
        case '\f':
            escaped += "\\f";
            break;
        case '\n':
            escaped += "\\n";
            break;
        case '\r':
            escaped += "\\r";
            break;
        case '\t':
            escaped += "\\t";
            break;
        default:
            escaped += c;
            break;
        }
    }
    return escaped;
}

std::string JsonRpc::createJsonObject(const std::map<std::string, std::string> &data)
{
    std::stringstream ss;
    ss << "{\n";

    bool first = true;
    for (const auto &[key, value] : data)
    {
        if (!first)
            ss << ",\n";

        // Check if value is already JSON (starts with { or [)
        if (value.front() == '{' || value.front() == '[' || value == "true" || value == "false" ||
            (value.front() >= '0' && value.front() <= '9'))
        {
            ss << "    \"" << escapeJson(key) << "\": " << value;
        }
        else
        {
            ss << "    \"" << escapeJson(key) << "\": \"" << escapeJson(value) << "\"";
        }
        first = false;
    }

    ss << "\n}";
    return ss.str();
}

std::string JsonRpc::createJsonArray(const std::vector<std::string> &items)
{
    std::stringstream ss;
    ss << "[\n";

    for (size_t i = 0; i < items.size(); ++i)
    {
        if (i > 0)
            ss << ",\n";
        ss << "    " << items[i];
    }

    ss << "\n]";
    return ss.str();
}

std::string JsonRpc::extractValue(const std::string &json, const std::string &key)
{
    std::string searchKey = "\"" + key + "\"";
    size_t keyPos = json.find(searchKey);
    if (keyPos == std::string::npos)
        return "";

    size_t colonPos = json.find(":", keyPos);
    if (colonPos == std::string::npos)
        return "";

    // Skip whitespace after colon
    size_t valueStart = colonPos + 1;
    while (valueStart < json.length() && (json[valueStart] == ' ' || json[valueStart] == '\t' || json[valueStart] == '\n'))
    {
        valueStart++;
    }

    if (valueStart >= json.length())
        return "";

    // Extract quoted string value
    if (json[valueStart] == '"')
    {
        size_t quoteEnd = json.find("\"", valueStart + 1);
        if (quoteEnd != std::string::npos)
        {
            return json.substr(valueStart + 1, quoteEnd - valueStart - 1);
        }
    }

    return "";
}

std::map<std::string, std::string> JsonRpc::parseParams(const std::string &paramsJson)
{
    std::map<std::string, std::string> params;

    // Simple key-value extraction for demonstration
    // In a real implementation, use a proper JSON parser
    size_t pos = 0;
    while (pos < paramsJson.length())
    {
        size_t keyStart = paramsJson.find("\"", pos);
        if (keyStart == std::string::npos)
            break;

        size_t keyEnd = paramsJson.find("\"", keyStart + 1);
        if (keyEnd == std::string::npos)
            break;

        std::string key = paramsJson.substr(keyStart + 1, keyEnd - keyStart - 1);

        size_t colonPos = paramsJson.find(":", keyEnd);
        if (colonPos == std::string::npos)
            break;

        size_t valueStart = colonPos + 1;
        while (valueStart < paramsJson.length() && (paramsJson[valueStart] == ' ' || paramsJson[valueStart] == '\t'))
        {
            valueStart++;
        }

        if (valueStart >= paramsJson.length())
            break;

        std::string value;
        if (paramsJson[valueStart] == '"')
        {
            size_t valueEnd = paramsJson.find("\"", valueStart + 1);
            if (valueEnd != std::string::npos)
            {
                value = paramsJson.substr(valueStart + 1, valueEnd - valueStart - 1);
                pos = valueEnd + 1;
            }
            else
            {
                break;
            }
        }
        else
        {
            // Non-quoted value
            size_t valueEnd = paramsJson.find_first_of(",}", valueStart);
            if (valueEnd != std::string::npos)
            {
                value = paramsJson.substr(valueStart, valueEnd - valueStart);
                // Trim whitespace
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                pos = valueEnd;
            }
            else
            {
                break;
            }
        }

        params[key] = value;
    }

    return params;
}
