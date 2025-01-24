cmake_minimum_required(VERSION 3.16)

project(luaumb LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(submodules/luau)

include_directories(submodules/base64/include)

add_executable(luaumb src/luaumb.cpp src/localizer.cpp src/bundle.cpp src/utils.cpp)

target_link_libraries(luaumb PRIVATE Luau.Ast Luau.Analysis Luau.CLI.lib)
