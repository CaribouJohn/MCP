cmake_minimum_required(VERSION 3.16)
project(mcp_server_cpp VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include FetchContent for downloading dependencies
include(FetchContent)


# Fetch nlohmann/json
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

# Fetch spdlog
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.13.0
)
FetchContent_MakeAvailable(spdlog)

# Find packages
find_package(Threads REQUIRED)

# Include directories
include_directories(include)

# Add executable
add_executable(mcp_server
    src/main.cpp
    src/mcp_server.cpp
    src/json_rpc.cpp
)

# Link libraries
target_link_libraries(mcp_server 
    Threads::Threads
    nlohmann_json::nlohmann_json
    spdlog::spdlog
)

# Set output directory
set_target_properties(mcp_server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Add compile options for better debugging
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(mcp_server PRIVATE -g -O0)
endif()
