cmake_minimum_required(VERSION 3.16)

project(luaumb LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(submodules/luau)

include_directories(submodules/base64/include)

add_executable(
    luaumb

    src/main.cpp
    src/path.cpp
    src/luaumb.cpp
    src/localizer.cpp
    src/bundle.cpp
    src/utils.cpp
)

if (MSVC)
    target_compile_options(luaumb PRIVATE /W4)
else()
    target_compile_options(luaumb PRIVATE -Wall -Wextra -Wpedantic)
endif()

if (NOT DEFINED LUAUMB_VERSION)
    set(LUAUMB_VERSION "0.1-dev")
endif()

target_compile_definitions(luaumb PRIVATE LUAUMB_VERSION="${LUAUMB_VERSION}")

target_link_libraries(luaumb PRIVATE Luau.Ast Luau.Analysis Luau.CLI.lib Luau.Config)
