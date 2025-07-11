#!/usr/bin/env python3
"""
Quick setup verification for VS Code MCP integration
"""

import json
import os
import subprocess


def check_mcp_setup():
    print("🔧 MCP Server Setup Verification\n")

    # Check if server executable exists
    server_path = r"C:\Development\MCP\build\bin\Debug\mcp_server.exe"
    if os.path.exists(server_path):
        print("✅ MCP server executable found")
    else:
        print("❌ MCP server executable NOT found")
        print(f"   Expected: {server_path}")
        return False

    # Test if server responds
    try:
        result = subprocess.run(
            [server_path],
            input='{"jsonrpc":"2.0","id":"1","method":"initialize","params":{}}',
            capture_output=True,
            text=True,
            timeout=5,
        )

        if result.returncode == 0 and "jsonrpc" in result.stdout:
            print("✅ MCP server responds correctly")
        else:
            print("❌ MCP server not responding properly")
            print(f"   stdout: {result.stdout[:100]}...")
            return False
    except Exception as e:
        print(f"❌ Error testing server: {e}")
        return False

    # Check VS Code settings
    print("\n📋 VS Code Configuration:")
    print("1. Open VS Code Settings (Ctrl+,)")
    print("2. Click 'Open Settings (JSON)' (top-right icon)")
    print("3. Add this configuration:")
    print(
        """
{
  "github.copilot.chat.experimental.mcp.servers": {
    "cpp-mcp-server": {
      "command": "C:\\\\Development\\\\MCP\\\\build\\\\bin\\\\Debug\\\\mcp_server.exe",
      "args": []
    }
  }
}
"""
    )

    print("\n🔄 Next Steps:")
    print("1. Add the configuration above to VS Code settings")
    print("2. Restart VS Code completely")
    print("3. Open Copilot Chat (Ctrl+Alt+I)")
    print('4. Test: @cpp-mcp-server echo "Hello World"')

    return True


if __name__ == "__main__":
    check_mcp_setup()
