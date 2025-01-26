# Luaumb - Luau module bundle

Bundle all source code modules called via `require` by wrapping them in functions.

## Build

```sh
git clone --recurse-submodules https://github.com/noonomyen/luaumb.git
cd luaumb
mkdir -p build && cd build
cmake .. && cmake --build . -j $(nproc)
```

## Usage

```txt
Luau module bundle - v0.1
  luaumb [main.luau] [out.luau]
```

### example

```txt
mkdir -p tmp
./luaumb ./example/main.luau ./tmp/out.luau

Modules and dependencies
  lib/count.luau
  lib/print.luau
  c.luau
  └─ [./lib/print] (lib/print.luau) -- 0:22-0:44
  └─ [./lib/count] (lib/count.luau) -- 1:16-1:38
  b.luau
  └─ [./lib/print] (lib/print.luau) -- 0:22-0:44
  └─ [./lib/count] (lib/count.luau) -- 1:16-1:38
  └─ [./c] (c.luau) -- 2:18-2:32
  lib/pi.luau
  a.luau
  └─ [./lib/print] (lib/print.luau) -- 0:22-0:44
  └─ [./lib/count] (lib/count.luau) -- 1:16-1:38
  └─ [./b] (b.luau) -- 2:18-2:32
  └─ [./c] (c.luau) -- 3:18-3:32
  main.luau
  └─ [./lib/print] (lib/print.luau) -- 0:22-0:44
  └─ [./a] (a.luau) -- 1:17-1:31
  └─ [./lib/pi] (lib/pi.luau) -- 18:11-18:30
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
