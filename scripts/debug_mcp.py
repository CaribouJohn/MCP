#!/usr/bin/env python3
"""
Debug script to see what the MCP server is actually outputting
"""

import subprocess
import json

def debug_mcp_server():
    server_path = r"C:\Development\MCP\build\bin\Debug\mcp_server.exe"
    
    try:
        process = subprocess.Popen(
            [server_path],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )
        
        # Send a simple initialize request
        init_request = {
            "jsonrpc": "2.0",
            "id": "1",
            "method": "initialize",
            "params": {}
        }
        
        request_str = json.dumps(init_request) + "\n"
        print(f"Sending: {request_str.strip()}")
        
        process.stdin.write(request_str)
        process.stdin.close()
        
        # Read all output
        stdout, stderr = process.communicate(timeout=5)
        
        print(f"STDOUT: '{stdout}'")
        print(f"STDERR: '{stderr}'")
        print(f"Return code: {process.returncode}")
        
    except Exception as e:
        print(f"Error: {e}")
        if 'process' in locals():
            process.terminate()

if __name__ == "__main__":
    debug_mcp_server()
