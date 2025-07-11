# MCP Server C++

A Model Context Protocol (MCP) server implementation in C++ for learning and experimentation.

## Overview

This project implements a basic MCP server that demonstrates the core concepts of the Model Context Protocol. The server supports the `tools` capability and includes several example tools to get you started.

## Features

- **JSON-RPC 2.0 Communication**: Full support for JSON-RPC protocol as required by MCP
- **Tools Capability**: Register and execute custom tools remotely
- **Robust JSON Handling**: Uses nlohmann/json library for reliable JSON parsing and generation
- **Extensible Architecture**: Easy to add new tools and capabilities
- **Cross-Platform**: Built with CMake for Windows, Linux, and macOS

## Built-in Tools

The server comes with three example tools:

1. **echo** - Echoes back any message you send
2. **get_time** - Returns the current system time
3. **system_info** - Shows server information and capabilities

## Building the Project

### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.16 or higher

### Build Steps

```bash
# Create build directory
mkdir build
cd build

# Configure the project
cmake ..

# Build the project
cmake --build .
```

### Running the Server

```bash
# From the build directory
./bin/mcp_server
```

## Project Structure

```
├── src/
│   ├── main.cpp          # Entry point
│   ├── mcp_server.cpp    # Main server implementation
│   └── json_rpc.cpp      # JSON-RPC handling
├── include/
│   ├── mcp_server.h      # Server class definition
│   └── json_rpc.h        # JSON-RPC utilities
├── CMakeLists.txt        # Build configuration
└── README.md            # This file
```

## Understanding MCP

The Model Context Protocol (MCP) is a standard for connecting AI assistants to external tools and data sources. This implementation shows how to:

1. **Initialize** - Establish connection and negotiate capabilities
2. **List Tools** - Advertise available tools to clients
3. **Execute Tools** - Handle tool calls and return results
4. **Handle Errors** - Proper error handling and reporting

## Adding New Tools

To add a new tool, modify the `setupDefaultTools()` method in `src/mcp_server.cpp`:

```cpp
// Add a new tool
McpTool myTool;
myTool.name = "my_tool";
myTool.description = "Description of what this tool does";
myTool.inputSchema["param1"] = "string";

addTool(myTool, [](const std::map<std::string, std::string>& params) -> std::string {
    // Your tool implementation here
    return "Tool result";
});
```

## Development

This is a learning project that demonstrates MCP concepts with nlohmann/json for robust JSON handling. For production use, consider:

- Adding proper input validation and error handling
- Implementing additional transport layers (HTTP, WebSocket)
- Adding logging and debugging capabilities
- Supporting additional MCP capabilities (resources, prompts)

## VS Code Integration

To use this MCP server with VS Code and GitHub Copilot Chat:

1. **Build the server**: `cmake --build build`
2. **Configure VS Code**: Add to your settings.json:
   ```json
   {
     "github.copilot.chat.experimental.mcp.servers": {
       "cpp-mcp-server": {
         "command": "C:\\Development\\MCP\\build\\bin\\Debug\\mcp_server.exe",
         "args": []
       }
     }
   }
   ```
3. **Restart VS Code** and test in Copilot Chat with commands like:
   - `@cpp-mcp-server echo "Hello!"`
   - `@cpp-mcp-server get_time`
   - `@cpp-mcp-server system_info`

See [VS_CODE_INTEGRATION.md](VS_CODE_INTEGRATION.md) for detailed setup instructions.

## Learn More

- [Model Context Protocol Specification](https://modelcontextprotocol.io/)
- [MCP Documentation](https://modelcontextprotocol.io/docs/)
- [Example Implementations](https://github.com/modelcontextprotocol)

## License

This project is for educational purposes. Feel free to modify and experiment!
