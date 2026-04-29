cmake_minimum_required(VERSION 3.16)

project(luau-type-remove LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(LUAU_TYPE_REMOVE_STATIC_LINUX "Build static binary on Linux" OFF)

set(LUAU_BUILD_CLI OFF CACHE BOOL "Build CLI" FORCE)
set(LUAU_BUILD_TESTS OFF CACHE BOOL "Build tests" FORCE)
set(LUAU_BUILD_WEB OFF CACHE BOOL "Build Web module" FORCE)
set(LUAU_WERROR OFF CACHE BOOL "Warnings as errors" FORCE)
set(LUAU_STATIC_CRT OFF CACHE BOOL "Static CRT" FORCE)
set(LUAU_EXTERN_C OFF CACHE BOOL "Extern C" FORCE)
set(LUAU_BUILD_SHARED OFF CACHE BOOL "Shared libs" FORCE)

add_subdirectory(submodules/luau)

add_executable(
    luau-type-remove
    luau-type-remove.cpp
)

if (MSVC)
    target_compile_options(luau-type-remove PRIVATE /W4)
else()
    target_compile_options(luau-type-remove PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_link_libraries(luau-type-remove PRIVATE Luau.Ast Luau.Analysis Luau.CLI.lib Luau.Config)

if (UNIX AND NOT APPLE AND LUAU_TYPE_REMOVE_STATIC_LINUX)
    target_link_options(luau-type-remove PRIVATE -static -static-libgcc -static-libstdc++)

    set_target_properties(luau-type-remove PROPERTIES
        LINK_SEARCH_START_STATIC ON
        LINK_SEARCH_END_STATIC ON
    )
endif()
