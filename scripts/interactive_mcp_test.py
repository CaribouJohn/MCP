#!/usr/bin/env python3
"""
Interactive MCP client to test the echo tool manually.
This demonstrates the exact format VS Code should use.
"""

import json
import subprocess
import threading
import sys
import time


class McpClient:
    def __init__(self, server_path):
        self.server_path = server_path
        self.process = None
        self.request_id = 0

    def start_server(self):
        """Start the MCP server process."""
        self.process = subprocess.Popen(
            [self.server_path],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            bufsize=1,
        )
        print("✅ MCP Server started")

    def send_request(self, method, params=None):
        """Send a JSON-RPC request to the server."""
        if params is None:
            params = {}

        self.request_id += 1
        request = {
            "jsonrpc": "2.0",
            "id": self.request_id,
            "method": method,
            "params": params,
        }

        print(f"\n📤 Sending: {method}")
        print(f"Request: {json.dumps(request, indent=2)}")

        # Send request
        self.process.stdin.write(json.dumps(request) + "\n")
        self.process.stdin.flush()

        # Read response
        response_line = self.process.stdout.readline()
        if response_line:
            try:
                response = json.loads(response_line.strip())
                print(f"📥 Response: {json.dumps(response, indent=2)}")
                return response
            except json.JSONDecodeError as e:
                print(f"❌ Failed to parse response: {e}")
                print(f"Raw response: {response_line}")
                return None
        else:
            print("❌ No response received")
            return None

    def initialize(self):
        """Initialize the MCP connection."""
        params = {
            "protocolVersion": "2024-11-05",
            "capabilities": {"tools": {}},
            "clientInfo": {"name": "interactive-test-client", "version": "1.0.0"},
        }
        return self.send_request("initialize", params)

    def list_tools(self):
        """List available tools."""
        return self.send_request("tools/list")

    def call_tool(self, name, arguments=None):
        """Call a specific tool."""
        if arguments is None:
            arguments = {}

        params = {"name": name, "arguments": arguments}
        return self.send_request("tools/call", params)

    def stop_server(self):
        """Stop the server process."""
        if self.process:
            self.process.stdin.close()
            self.process.terminate()
            self.process.wait(timeout=5)
            print("✅ MCP Server stopped")


def main():
    print("🔧 Interactive MCP Client")
    print("=" * 50)

    client = McpClient("c:\\Development\\MCP\\build\\bin\\Debug\\mcp_server.exe")

    try:
        # Start server
        client.start_server()
        time.sleep(0.5)  # Give server time to start

        # Initialize
        print("\n🚀 Step 1: Initialize")
        init_response = client.initialize()
        if not init_response or "error" in init_response:
            print("❌ Initialization failed!")
            return

        # List tools
        print("\n📋 Step 2: List Tools")
        tools_response = client.list_tools()
        if not tools_response or "error" in tools_response:
            print("❌ Tools list failed!")
            return

        # Test echo tool
        print("\n🔊 Step 3: Test Echo Tool")
        echo_response = client.call_tool(
            "echo", {"message": "Hello from interactive test!"}
        )
        if echo_response and "result" in echo_response:
            content = echo_response["result"]["content"]
            if isinstance(content, list) and len(content) > 0:
                text = content[0].get("text", "")
                print(f"✅ Echo successful: {text}")
            else:
                print("❌ Echo response format invalid")
        else:
            print("❌ Echo call failed!")

        # Test get_time tool
        print("\n⏰ Step 4: Test Get Time Tool")
        time_response = client.call_tool("get_time")
        if time_response and "result" in time_response:
            content = time_response["result"]["content"]
            if isinstance(content, list) and len(content) > 0:
                text = content[0].get("text", "")
                print(f"✅ Time successful: {text}")
            else:
                print("❌ Time response format invalid")
        else:
            print("❌ Time call failed!")

        # Test system_info tool
        print("\n💻 Step 5: Test System Info Tool")
        info_response = client.call_tool("system_info")
        if info_response and "result" in info_response:
            content = info_response["result"]["content"]
            if isinstance(content, list) and len(content) > 0:
                text = content[0].get("text", "")
                print(f"✅ System info successful: {text}")
            else:
                print("❌ System info response format invalid")
        else:
            print("❌ System info call failed!")

        print("\n🎉 All tests completed!")

    except KeyboardInterrupt:
        print("\n🛑 Interrupted by user")
    except Exception as e:
        print(f"\n❌ Error: {e}")
    finally:
        client.stop_server()


if __name__ == "__main__":
    main()
