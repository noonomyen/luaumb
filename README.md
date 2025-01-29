# Luaumb - Luau module bundle

Bundle all source code modules called via `require` by wrapping them in functions.

## Build

```sh
git clone --recurse-submodules https://github.com/noonomyen/luaumb.git
cd luaumb
mkdir -p build && cd build
cmake .. && cmake --build . -j $(nproc)
```

## Download from last build CI

This is a temporary workaround because some commits may fail to build or have bugs that cause the program to malfunction.

- [LICENSE.txt](https://cdn.noonomyen.com/static/repo/luaumb/ci-last-build/LICENSE.txt)

| Build | OS | Compiler | Link |
| :- | :- | :- | :- |
| Release | Ubuntu | GCC | [luaumb](https://cdn.noonomyen.com/static/repo/luaumb/ci-last-build/Release/ubuntu-latest/gcc/luaumb) |
| Release | Ubuntu | Clang | [luaumb](https://cdn.noonomyen.com/static/repo/luaumb/ci-last-build/Release/ubuntu-latest/clang/luaumb) |
| Release | Windows | MSVC | [luaumb.exe](https://cdn.noonomyen.com/static/repo/luaumb/ci-last-build/Release/windows-latest/cl/luaumb.exe) |

## Usage

```txt
Luau module bundle - 0.1-dev luau:0.658-release base64:387b32f-checkout
  luaumb [main.luau] [out.luau]
```

### example

```txt
mkdir -p tmp
./luaumb ./example/main.luau ./tmp/out.luau

Modules and dependencies
  /lib/count.luau
  /lib/print.luau
  /hello/world.luau
  /c.luau
  └─ [./lib/print] (/lib/print.luau) -- 0:22-0:44
  └─ [./lib/count] (/lib/count.luau) -- 1:16-1:38
  /good/space/space.luau
  /hello/space/init.luau
  └─ [@w] (/hello/world.luau) -- 1:19-1:32
  /b.luau
  └─ [./lib/print] (/lib/print.luau) -- 0:22-0:44
  └─ [./lib/count] (/lib/count.luau) -- 1:16-1:38
  └─ [./c] (/c.luau) -- 2:18-2:32
  /good/space/world/init.luau
  └─ [@space] (/good/space/space.luau) -- 1:22-1:39
  /hello/init.luau
  └─ [@s] (/hello/space/init.luau) -- 1:23-1:36
  /lib/pi.luau
  /a.luau
  └─ [./lib/print] (/lib/print.luau) -- 0:22-0:44
  └─ [./lib/count] (/lib/count.luau) -- 1:16-1:38
  └─ [./b] (/b.luau) -- 2:18-2:32
  └─ [./c] (/c.luau) -- 3:18-3:32
  /main.luau
  └─ [@print] (/lib/print.luau) -- 0:22-0:39
  └─ [./a] (/a.luau) -- 1:17-1:31
  └─ [./lib/pi] (/lib/pi.luau) -- 13:11-13:30
  └─ [./hello] (/hello/init.luau) -- 20:6-20:24
  └─ [./good/space/world] (/good/space/world/init.luau) -- 21:6-21:35
```

## How It Works

The process begins by searching for locations where the `require` function is called using the Luau AST (Abstract Syntax Tree). The arguments provided to these calls are then used to locate the corresponding module files. Once all module files are identified, their dependency order for loading is determined.

To create the output file, the source code of each module is wrapped in an anonymous function, with lazy initialization used for invoking these functions. In the source code of each module, instances of the `require` function are replaced by the corresponding wrapped function. The main file is also treated as a module to ensure that local variables declared in the main file are not accessible in other modules' local scopes once the bundle is created.

`NAME` is generated from the main file to the relative file path, encoded in Base64

- `_LUAUMB_MODULE_NAME` stores the anonymous function.
- `_LUAUMB_LOADED_NAME` stores the return value of the function on its first execution.
- `_LUAUMB_NAME` acts as the `require` function (returning `_LUAUMB_LOADED_NAME`).

## `require` function

As you know, luaumb will replace the position of the code where the `require` function is called. It looks for a global `require` function with a single argument that is a constant string. If you want luaumb to skip or ignore the `require` call at that position, you can use something that is not exactly a constant string, such as using `require(("./module"))`. The parser will see that there's a group within the arguments, and the constant string is within that group, causing luaumb to ignore this position. or `_G.require("./module")` The parser will see that the `require` function is not global.

## Dependencies

- [luau-lang/luau](https://github.com/luau-lang/luau)
- [tobiaslocker/base64](https://github.com/tobiaslocker/base64)
