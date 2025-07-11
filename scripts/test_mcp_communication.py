#!/usr/bin/env python3
"""
Test script to verify MCP server communication via stdin/stdout
"""

import subprocess
import json
import sys

def test_mcp_server():
    # Start the MCP server process
    server_path = r"C:\Development\MCP\build\bin\Debug\mcp_server.exe"
    
    try:
        process = subprocess.Popen(
            [server_path],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            bufsize=1
        )
        
        # Test 1: Initialize
        print("Testing initialize...")
        init_request = {
            "jsonrpc": "2.0",
            "id": "1",
            "method": "initialize",
            "params": {
                "protocolVersion": "2024-11-05",
                "capabilities": {"tools": {}},
                "clientInfo": {"name": "test-client", "version": "1.0.0"}
            }
        }
        
        process.stdin.write(json.dumps(init_request) + "\n")
        process.stdin.flush()
        
        response = process.stdout.readline()
        if response:
            print("Initialize response:", json.dumps(json.loads(response), indent=2))
        
        # Test 2: List tools
        print("\nTesting tools/list...")
        list_request = {
            "jsonrpc": "2.0",
            "id": "2",
            "method": "tools/list",
            "params": {}
        }
        
        process.stdin.write(json.dumps(list_request) + "\n")
        process.stdin.flush()
        
        response = process.stdout.readline()
        if response:
            print("List tools response:", json.dumps(json.loads(response), indent=2))
        
        # Test 3: Call echo tool
        print("\nTesting tools/call (echo)...")
        call_request = {
            "jsonrpc": "2.0",
            "id": "3",
            "method": "tools/call",
            "params": {
                "name": "echo",
                "arguments": {"message": "Hello from MCP test!"}
            }
        }
        
        process.stdin.write(json.dumps(call_request) + "\n")
        process.stdin.flush()
        
        response = process.stdout.readline()
        if response:
            print("Echo response:", json.dumps(json.loads(response), indent=2))
        
        # Close the process
        process.stdin.close()
        process.terminate()
        process.wait()
        
        print("\n✅ MCP server communication test completed successfully!")
        
    except Exception as e:
        print(f"❌ Error testing MCP server: {e}")
        if 'process' in locals():
            process.terminate()

if __name__ == "__main__":
    test_mcp_server()
