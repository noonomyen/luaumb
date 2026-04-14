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
luaumb [main.luau] [out.luau]
```

## How It Works

The process begins by searching for locations where the `require` function is called using the Luau AST (Abstract Syntax Tree). The arguments provided to these calls are then used to locate the corresponding module files. Once all module files are identified, their dependency order for loading is determined.

To create the output file, the source code of each module is wrapped in an anonymous function, with lazy initialization used for invoking these functions. In the source code of each module, instances of the `require` function are replaced by the corresponding wrapped function. The main file is also treated as a module to ensure that local variables declared in the main file are not accessible in other modules' local scopes once the bundle is created.

- `__LUAUMB` is a table used to store all luaumb-related data.
- `.METATABLE` is the metatable used to set for all wrapper functions.
- `.MODULES` is a table that stores wrapper functions representing module files.
- `.LOADED` is a table that stores the return values of wrapper functions after they have been executed.
- `.LOAD` is a function used to load a module, acting similarly to `require(...)`.

## `require` function

As you know, luaumb will replace the position of the code where the `require` function is called. It looks for a global `require` function with a single argument that is a constant string. If you want luaumb to skip or ignore the `require` call at that position, you can use something that is not exactly a constant string, such as using `require(("./module"))`. The parser will see that there's a group within the arguments, and the constant string is within that group, causing luaumb to ignore this position. or `_G.require("./module")` The parser will see that the `require` function is not global.

## Dependencies

- [luau-lang/luau](https://github.com/luau-lang/luau)
