# Luaumb - Luau module bundle

Bundle all source code modules called via `require` by wrapping them in functions.

## Build

```sh
git clone --recurse-submodules https://github.com/noonomyen/luaumb.git
cd luaumb
mkdir build && cd build
cmake .. && make -j $(nproc)
```

## Usage

```txt
Luau module bundle - v0.1
  luaumb [main.luau] [out.luau]
```

### example

```sh
mkdir tmp
./luaumb ./example/main.luau ./tmp/out.luau
```

## How It Works

The process begins by searching for locations where the `require` function is called using the Luau AST (Abstract Syntax Tree). The arguments provided to these calls are then used to locate the corresponding module files. Once all module files are identified, their dependency order for loading is determined.

To create the output file, the source code of each module is wrapped in an anonymous function, with lazy initialization used for invoking these functions. In the source code of each module, instances of the `require` function are replaced by the corresponding wrapped function. The main file is also treated as a module to ensure that local variables declared in the main file are not accessible in other modules' local scopes once the bundle is created.

`NAME` is generated from the main file to the relative file path, encoded in Base64

- `_LUAUMB_MODULE_NAME` stores the anonymous function.
- `_LUAUMB_LOADED_NAME` stores the return value of the function on its first execution.
- `_LUAUMB_NAME` acts as the `require` function (returning `_LUAUMB_LOADED_NAME`).

## Dependencies

- [luau-lang/luau](https://github.com/luau-lang/luau)
- [tobiaslocker/base64](https://github.com/tobiaslocker/base64)
