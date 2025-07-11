# Using Your C++ MCP Server with VS Code

This guide shows you how to integrate your C++ MCP server with VS Code and GitHub Copilot Chat.

## 🚀 Quick Start

Your MCP server is ready! Follow these steps to use it:

1. **Open VS Code Settings**: Press `Ctrl+,`
2. **Open Settings JSON**: Click the "Open Settings (JSON)" icon (top-right)
3. **Add Configuration**: Insert this JSON:
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
4. **Restart VS Code**: Close and reopen VS Code completely
5. **Test**: Open Copilot Chat (`Ctrl+Alt+I`) and try:
   ```
   @cpp-mcp-server echo "Hello World!"
   ```

## Prerequisites

1. **VS Code with GitHub Copilot Extension** - Make sure you have the GitHub Copilot extension installed
2. **Built MCP Server** - Your `mcp_server.exe` should be built and working

## Step 1: Configure VS Code Settings

You need to add your MCP server to VS Code's settings. There are two ways to do this:

### Option A: User Settings (Recommended)

1. Open VS Code Settings (`Ctrl+,`)
2. Click on "Open Settings (JSON)" (icon in top-right)
3. Add the following configuration:

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

### Option B: Workspace Settings

1. In your project root, create/edit `.vscode/settings.json`
2. Add the same configuration as above

## Step 2: Restart VS Code

After adding the configuration:

1. Restart VS Code completely
2. The MCP server should automatically start when you open Copilot Chat

## Step 3: Test in Copilot Chat

1. Open GitHub Copilot Chat (`Ctrl+Alt+I` or click the chat icon)
2. Try these test commands:

### Test the Echo Tool

```
@cpp-mcp-server echo "Hello from VS Code!"
```

### Test the Time Tool

```
@cpp-mcp-server get_time
```

### Test the System Info Tool

```
@cpp-mcp-server system_info
```

## Step 4: Verify Connection

To check if your MCP server is connected:

1. Look in VS Code's Output panel
2. Select "GitHub Copilot Chat" from the dropdown
3. You should see connection messages

## Troubleshooting

### Common Issues

1. **VS Code not recognizing the server**

   - **Path verification**: Check that the path to `mcp_server.exe` is correct
   - **Restart required**: You MUST restart VS Code completely after configuration
   - **Dual configuration**: Try both user settings AND workspace `.vscode/mcp.json`

2. **MCP server shows as "stopped" in VS Code**

   - VS Code can see the server but can't start it
   - **Check path**: Ensure the executable path is correct and accessible
   - **Try absolute path**: Use full path: `C:\\Development\\MCP\\build\\bin\\Debug\\mcp_server.exe`
   - **Run as administrator**: Try starting VS Code as administrator
   - **Check permissions**: Ensure VS Code can execute the file
   - **Working directory**: Try adding `"cwd": "C:\\Development\\MCP"` to config

3. **VS Code expecting initialize response**

   - Your server is being contacted but VS Code can't complete the handshake
   - Make sure no other instances of the server are running: `taskkill /F /IM mcp_server.exe`
   - Try the workspace configuration method (`.vscode/mcp.json`) instead

4. **Permission errors**

   - Ensure the executable has proper permissions
   - Try running VS Code as administrator if needed

5. **Connection timeouts**
   - Check Windows firewall settings
   - Verify the server responds to stdin/stdout (use our test scripts)

### Debug Commands

Test the server manually:

```bash
# Test basic communication
python test_mcp_communication.py

# Test initialization specifically
python test_initialization.py

# Check if server is running
tasklist | findstr mcp_server
```

### VS Code MCP Commands

Use these commands in VS Code to debug:

1. **Ctrl+Shift+P** → "MCP: List Servers" (should show your server)
2. **Ctrl+Shift+P** → "MCP: Add Server..." (alternative way to add)
3. **View** → **Output** → Select "GitHub Copilot Chat" (check for errors)

### Alternative Configuration Method

If the user settings don't work, use workspace configuration:

1. Create `.vscode/mcp.json` in your project root:

```json
{
  "servers": {
    "cpp-mcp-server": {
      "command": "C:\\Development\\MCP\\build\\bin\\Debug\\mcp_server.exe",
      "args": []
    }
  }
}
```

2. Restart VS Code completely

### **🔧 VS Code Initialization Issues**

If you see VS Code waiting for `initialize` response but the server logs show responses being sent:

```
2025-07-08 20:58:56.695 [warning] [server stderr] [2025-07-08 20:58:56] [DEBUG] Sending response #1: {"id":"1"...
2025-07-08 20:59:01.696 [info] Waiting for server to respond to `initialize` request...
```

This indicates a **buffering or ID type mismatch issue**. The fix involves:

1. **ID Type Preservation**: VS Code sends numeric IDs but expects numeric IDs back
2. **Output Buffering**: Ensure stdout is immediately flushed

**Fixed in the current server version** - the server now:

- Preserves numeric ID types in JSON-RPC responses
- Disables stdout buffering for immediate response delivery
- Properly handles both string and numeric request IDs

If you still see this issue:

1. Rebuild the server: `cmake --build build`
2. Restart VS Code completely
3. Check that you're using the updated executable

# How VS Code MCP Communication Works

## Understanding the Communication Flow

VS Code's MCP integration works through a specific process:

### 1. **Server Startup Process**

When you configure an MCP server in VS Code:

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

VS Code will:

1. **Launch the executable** as a child process when needed
2. **Connect to stdin/stdout** of that process for communication
3. **Send JSON-RPC messages** via stdin
4. **Receive responses** via stdout

### 2. **Communication Protocol**

The communication uses **JSON-RPC 2.0** over **stdin/stdout**:

```
VS Code                          Your MCP Server
   |                                     |
   |  {"method": "initialize", ...}      |
   |------------------------------------>| (via stdin)
   |                                     |
   |  {"result": {...}}                  |
   |<------------------------------------| (via stdout)
   |                                     |
   |  {"method": "tools/list", ...}      |
   |------------------------------------>|
   |                                     |
   |  {"result": {"tools": [...]}}       |
   |<------------------------------------|
```

### 3. **Your Server's Role**

Your C++ server must:

- **Read from stdin** line by line (JSON-RPC requests)
- **Write to stdout** line by line (JSON-RPC responses)
- **Stay alive** and keep listening for new requests
- **Never write** anything else to stdout (logs go to stderr)

### 4. **Why Your Server Shows as "Stopped"**

This happens when:

- **Process exits immediately** - Server crashes or exits after startup
- **No response to initialize** - Server doesn't respond to first request
- **Stdout pollution** - Server writes non-JSON to stdout
- **Buffering issues** - Server doesn't flush stdout properly

### 5. **How VS Code Detects "Running" vs "Stopped"**

- **Running**: Process is alive AND responds to `initialize` request
- **Stopped**: Process exited OR failed to respond to `initialize`

## Debugging the Communication

### Check What VS Code Sees

1. **View** → **Output** → Select **"GitHub Copilot Chat"**
2. Look for messages like:
   ```
   [MCP] Starting server: cpp-mcp-server
   [MCP] Server cpp-mcp-server failed to initialize
   [MCP] Server cpp-mcp-server stopped unexpectedly
   ```

### Test Server Manually

Your server should work like this:

```bash
# Start the server
C:\Development\MCP\build\bin\Debug\mcp_server.exe

# Type this JSON (then press Enter):
{"jsonrpc":"2.0","id":"1","method":"initialize","params":{"protocolVersion":"2024-11-05"}}

# Server should respond with:
{"id":"1","jsonrpc":"2.0","result":{"capabilities":{"logging":true,"tools":true},"protocolVersion":"2024-11-05","serverInfo":{"name":"cpp-mcp-server","version":"1.0.0"}}}
```

### Common Issues with Communication

1. **Server exits immediately**

   - Check for missing DLLs or runtime errors
   - Run server manually to see error messages

2. **Server doesn't respond**

   - Check if server is reading from stdin properly
   - Verify JSON parsing is working

3. **VS Code can't start server**
   - Path issues
   - Permission problems
   - Working directory issues

## 🔍 **Debug Logging**

Your MCP server now includes comprehensive debug logging to help troubleshoot issues:

### **Where Logs Go**

1. **Console (stderr)**: Real-time logs visible when running server manually
2. **Log File**: `C:\Development\MCP\mcp_server.log` - Persistent logs

### **Log Levels**

- **DEBUG**: Detailed request/response information
- **INFO**: Important events (server start, method calls)
- **WARN**: Warning conditions
- **ERROR**: Error conditions

### **Viewing Logs**

#### Real-time Debugging

```bash
# Run server manually to see real-time logs
C:\Development\MCP\build\bin\Debug\mcp_server.exe

# Logs will appear on stderr while JSON responses go to stdout
```

#### Log File Analysis

```bash
# View the log file
Get-Content C:\Development\MCP\mcp_server.log

# Monitor log file in real-time (PowerShell)
Get-Content C:\Development\MCP\mcp_server.log -Wait
```

### **VS Code Integration Debugging**

When VS Code shows your server as "stopped", check:

1. **VS Code Output Panel**: **View** → **Output** → Select **"GitHub Copilot Chat"**
2. **Server Log File**: Check `C:\Development\MCP\mcp_server.log` for what happened
3. **Manual Test**: Run server manually to see real-time error messages

### **Common Log Patterns**

**Successful Startup:**

```
[INFO] === MCP Server Starting ===
[INFO] Initializing MCP server: cpp-mcp-server v1.0.0
[INFO] Server initialization complete. Tools registered: 3
[INFO] Server initialized, starting main communication loop
```

**Successful Request:**

```
[DEBUG] Received request #1: {"jsonrpc": "2.0", "id": "1", "method": "initialize"...
[INFO] Parsed request - Method: initialize, ID: 1
[INFO] Handling initialize request
[INFO] Initialize response created successfully
[DEBUG] Sending response #1: {"id":"1","jsonrpc":"2.0","result":...
```

**Problem Indicators:**

```
[ERROR] Failed to parse JSON-RPC request: ...
[WARN] Unknown method: ...
[ERROR] Exception in main: ...
```

---

## What You Can Do Now

With your MCP server connected, you can:

1. **Use tools in chat**: Ask Copilot to use your custom tools
2. **Get current time**: Ask for system time from your C++ server
3. **Echo messages**: Test communication with the echo tool
4. **System info**: Get information about your MCP server

## Adding More Tools

To add new tools to your server:

1. Edit `src/mcp_server.cpp` in the `setupDefaultTools()` method
2. Add your new tool with proper JSON schema
3. Rebuild the server: `cmake --build build`
4. Restart VS Code to pick up changes

## Example Usage in Chat

Once configured, you can use commands like:

- "Use the cpp-mcp-server to echo 'Hello World'"
- "What's the current time according to the cpp-mcp-server?"
- "Show me system information from the cpp-mcp-server"

Your C++ MCP server is now fully integrated with VS Code and ready to extend Copilot's capabilities!

### **🚨 Tool Calls Not Working?**

If VS Code can see your server and `tools/list` works, but tool calls don't reach your server, try:

#### **1. Agent Mode Requirements**

- Switch to **Agent mode** in Copilot Chat (not Ask mode)
- In Agent mode, try: `Use the echo tool with message "Hello"`

#### **2. Alternative Syntax Patterns**

Try these different ways to invoke your tools:

```
# Direct tool reference
#echo Hello from VS Code

# MCP server prefix
/mcp.cpp-mcp-server.echo Hello

# Natural language in Agent mode
Please use the echo tool to say "Hello from VS Code"

# Tool mention with parameters
@cpp-mcp-server call echo with message "Hello"
```

#### **3. Check VS Code's Tool Integration**

1. **Ctrl+Shift+P** → "Chat: Configure Tools"
2. Look for your MCP server tools
3. Enable/disable them to refresh the connection

#### **4. Enable All Tools in Agent Mode**

1. In Agent mode, click the tools/configuration button
2. Make sure MCP tools are enabled
3. Try again with: `Use echo tool to say hello`

#### **5. Check for Updated VS Code Syntax**

The MCP integration is experimental, so syntax might change:

- Try with `#` prefix: `#cpp-mcp-server.echo "Hello"`
- Try slash commands: `/echo Hello`
- Check VS Code release notes for latest MCP syntax

#### **6. Monitor Real-time for Tool Calls**

Run this to watch for any tool calls:

```bash
# Monitor log file for any new requests
Get-Content C:\Development\MCP\mcp_server.log -Wait
```

Then try different tool invocation methods and watch if any `tools/call` requests appear.

### **🔇 Tool Works But No Response Shown in Chat**

If you see successful tool calls in the server logs but don't see responses in Copilot Chat:

```log
[INFO] Tool call completed successfully: echo
[DEBUG] Sending response #2: {"id":"2","jsonrpc":"2.0","result":{"content":[{"text":"Echo: Hello from VS Code!","type":"text"}]}}
```
