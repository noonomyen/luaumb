cmake_minimum_required(VERSION 3.16)

project(luaumb LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(LUAUMB_STATIC_LINUX "Build static binary on Linux" OFF)

set(LUAU_BUILD_CLI OFF CACHE BOOL "Build CLI" FORCE)
set(LUAU_BUILD_TESTS OFF CACHE BOOL "Build tests" FORCE)
set(LUAU_BUILD_WEB OFF CACHE BOOL "Build Web module" FORCE)
set(LUAU_WERROR OFF CACHE BOOL "Warnings as errors" FORCE)
set(LUAU_STATIC_CRT OFF CACHE BOOL "Static CRT" FORCE)
set(LUAU_EXTERN_C OFF CACHE BOOL "Extern C" FORCE)
set(LUAU_BUILD_SHARED OFF CACHE BOOL "Shared libs" FORCE)

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

target_link_libraries(luaumb PRIVATE Luau.Ast Luau.Analysis Luau.CLI.lib Luau.Config)

if (UNIX AND NOT APPLE AND LUAUMB_STATIC_LINUX)
    target_link_options(luaumb PRIVATE -static -static-libgcc -static-libstdc++)

    set_target_properties(luaumb PROPERTIES
        LINK_SEARCH_START_STATIC ON
        LINK_SEARCH_END_STATIC ON
    )
endif()
