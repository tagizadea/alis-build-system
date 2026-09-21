<p align="center">
  <img src="icon.png" alt="Icon" width="128">
</p>

# Ali's Build System (ABS)

A custom build system for C/C++ projects with its own scripting language (`.abs`). ABS lets you define your build workflow as a script — track source files, detect changes, compile in parallel, and link — all from a single declarative script.

> **Status:** v1.0.0-beta — all core features implemented

---

## What is ABS?

ABS is a build system that:

- **Tracks source files** and detects when they change (using content hashing, mtime, and file size)
- **Scans `#include` dependencies** to build a dependency graph
- **Invalidates dirty files** — when a header changes, all source files that depend on it (directly or transitively) are marked for recompilation
- **Compiles in parallel** — uses all available CPU cores
- **Caches results** — unchanged files are not recompiled
- **Links** object files into an executable
- **Extends via plugins** — compiled shared libraries that register native functions
- **Debugs itself** — multi-language error reporting with script file/line/column context

The build workflow is written in **ABS**, a small scripting language with variables, functions, conditionals, loops, lists, and objects.

---

## Building ABS

```bash
make
```

This compiles the ABS interpreter and runs it with `main.abs` (the default build script).

### Build Options

```bash
make ABS_LANG=AZ        # Azerbaijani debug messages (default: EN)
make ABS_PROFILE=1      # Enable performance profiling
make ABS_32BIT=1        # Force 32-bit mode (auto-detected on 32-bit platforms)
```

---

## How It Works

### 1. The Build Script (`main.abs`)

ABS looks for a `.abs` file in the current directory. The default is `main.abs`, or you can pass a script name as an argument:

```bash
./abs mybuild.abs
```

### 2. Tracking Sources

The `track()` function registers source files and detects which ones changed. Pass the files either as a list or as variadic arguments:

```abs
let dirty = track(["src/main.cpp", "src/manager.cpp", "src/operations.cpp"]);
```

`track()` returns a list of files that need recompilation (dirty files). It also scans all `#include` directives to build a dependency graph. If a header changes, `track()` uses **reverse invalidation** to find every source file that depends on it — even transitively.

> **Note:** a first-ever call returns all registered files as dirty; on later runs only changed files (or files affected by changed headers) are returned. Empty input (`track()` or `track([])`) returns an empty list.

### 3. Compiling

The `compile()` function compiles source files into object files. It takes a single object argument (or a list of them):

```abs
let objs = compile({
    compiler_path: "g++-15",
    src: ["src/main.cpp", "src/manager.cpp", "src/operations.cpp"],
    tracked_src: dirty,          # files that must be recompiled
    out_dir: "build",            # where .o files go
    flag: "-std=c++20 -I./include"
});
```

All five keys (`compiler_path`, `src`, `tracked_src`, `out_dir`, `flag`) are required, and both `src` and `tracked_src` must be lists of strings.

**How it decides what to compile:**

| Condition | Action |
|-----------|--------|
| File is in `tracked_src` (dirty) | Always compile |
| Object file exists AND `build_hash` matches cache | Skip (up-to-date) |
| Object file missing OR hash changed OR flags changed | Compile |

The `build_hash` combines the source file's content hash with a hash of the compiler flags — so changing flags (e.g. `-O2` → `-O0`) triggers a recompile even if the source didn't change.

**Parallel compilation:** All files that need compiling are compiled **simultaneously**, limited by the number of CPU cores (`std::thread::hardware_concurrency()`).

`compile()` returns a list of all object file paths (both freshly compiled and already up-to-date) — ready for linking.

### 4. Linking

The `link()` function links object files into an executable:

```abs
let exe = link(objs, "myapp", "g++-15");
```

- `objs` — list of object file paths (from `compile()`), all strings
- `myapp` — name of the output executable
- `g++-15` — compiler/linker to use (must exist on `PATH`)

`link()` links with `<compiler> <obj1> <obj2> ... -o <name>` and returns the executable path.

---

## The ABS Language

ABS is a small scripting language. Key syntax rules:

- `let` / `const` declarations **require** a trailing semicolon.
- Expression statements (function calls, assignments, member calls) take **no** semicolon.
- Functions have **no `return` keyword** — the last expression in the body is the return value.
- Blocks (`if`/`else`, `for`, `while`, `fn`) use `{` ... `}` braces.
- Comments start with `#` and run to end of line.
- String literals do **not** support escape sequences (`\"`, `\n`, …) — avoid them inside strings.

```abs
# Variables (semicolon required)
let x = 5;
const y = "hello";
let a = 1, b = 2, c = 3;

# Functions (implicit return: last expression is the return value)
fn add(a, b) {
    a + b
}

# Conditionals
if (x > 3) {
    print("x is big")
} else {
    print("x is small")
}

# Loops
for (let i = 0; i < 10; ++i) {
    print(i, " ")
}

while (x > 0) {
    x = x - 1
}

# Lists
let list = [1, 2, 3];
list.push(4)
print(list.size())

# Objects
let config = {
    compiler_path: "g++-15",
    flag: "-O2"
};
print(config.compiler_path)
```

---

## Native Functions

| Function | Description |
|----------|-------------|
| `track(...)` | Register source files, detect changes, return dirty files |
| `compile(config)` | Compile sources to object files (parallel, cached) |
| `link(objects, name, compiler)` | Link object files into an executable |
| `glob(dir, ext)` | Find all files with a given extension in a directory |
| `load_plugin(name)` | Load a compiled plugin from `.abs/plugins/` |
| `run(script)` | Run another ABS script, returns its environment as an object |
| `set_include(path)` | Add an include search path |
| `define(name, value?)` | Define a preprocessor macro |
| `print(...)` | Print values |
| `type(value)` | Get the type of a value: `"Number"`, `"String"`, `"List"`, `"Object"`, `"Bool"`, `"Func"`, `"NativeFunc"`, `"Null"` |
| `StoN(str)` | Convert string to number |
| `NtoS(num)` | Convert number to string |
| `floor(num)` | Floor a number |
| `max(list)` / `min(list)` | Max/min of a list |
| `system(cmd)` | Run a shell command, returns its exit status (or `Null` on failure) |
| `timeNow()` | Current time (minutes since midnight) |
| `set_lang("EN"/"AZ")` | Change debug message language at runtime |
| `debug_level("INFO")` | Set debug output severity threshold |
| `debug_log(msg)` | Log a message through the debug system |

---

## Plugin System

ABS supports compiled plugins — shared libraries that register native functions into the ABS environment.

### Plugin Format

A plugin is a shared library (`.so`/`.dylib`/`.dll`) that exports one function:

```c
// myplugin.cpp
#include "env.hpp"
#include "valtypes.hpp"
#include "operations.hpp"

std::shared_ptr<Value> my_function(std::vector<std::shared_ptr<Value>> args, Env* env){
    return Make_String("Hello from plugin!");
}

extern "C" void abs_plugin_init(Env* env){
    FunctionCall call;
    call.funAddr = my_function;
    env->declareVar("my_function", Make_NFunc(call), true);
    env->declareVar("my_var", Make_String("value"), true);
}
```

### Compiling a Plugin

```bash
# macOS
g++-15 -shared -fPIC myplugin.cpp -o .abs/plugins/myplugin.so -I./include -std=c++20 -undefined dynamic_lookup

# Linux
g++ -shared -fPIC myplugin.cpp -o .abs/plugins/myplugin.so -I./include -std=c++20

# Windows
cl /LD myplugin.cpp /Fe:.abs/plugins/myplugin.dll /I./include
```

### Using a Plugin

```abs
load_plugin("myplugin")
print(my_var, endl)        # "value"
print(my_function(), endl) # "Hello from plugin!"
```

Plugins are placed in `.abs/plugins/` (created automatically on first run).

> **Note:** a plugin `.so`/`.dylib` compiled against one interpreter build may need recompiling after you rebuild `main`, since the C++ ABI of `Env`/`Value` can change between revisions.

---

## Debug System

ABS has a built-in multi-language debug system:

- **Severity levels:** TRACE, DEBUG, INFO, WARNING, ERROR, FATAL
- **Categories:** LEXER, PARSER, EVAL, ENV, MANAGER, OPS, MEMORY, PERF, DSL
- **Languages:** English (default) and Azerbaijani, selectable at compile time (`make ABS_LANG=AZ`) or runtime (`set_lang("AZ")`)
- **Script context:** errors show the `.abs` file path and line:column where the problem occurred
- **Profiling:** opt-in via `make ABS_PROFILE=1`, shows per-function timing at exit

Example error output:
```
[FATAL][ENV] src/env.cpp:29 (resolve) | ./main.abs:26:11 | Variable "src" does not exist [env.resolve_not_found]
```

---

## Caching

ABS stores its state in the `.abs/` directory:

| File | Contents |
|------|----------|
| `.abs/files.cache` | File metadata (content hash, mtime, size) |
| `.abs/dependencies.cache` | Include dependency graph |
| `.abs/objects.cache` | Object file build hashes |
| `.abs/plugins/` | Compiled plugin libraries |

If you change the build system itself or suspect stale cache, delete the `.abs/` directory to start fresh:

```bash
rm -rf .abs
```

---

## Portability

ABS targets 32-bit and 64-bit systems with C++20 support. The `ABS_32BIT` define (auto-detected) switches to 32-bit-safe cache serialization and `double` instead of `long double`. POSIX and Windows are first-class platforms; other platforms can be ported by the community.