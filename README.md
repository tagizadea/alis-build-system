# Ali's Build System (ABS)

A custom build system for C/C++ projects with its own scripting language (`.abs`). ABS lets you define your build workflow as a script — track source files, detect changes, compile in parallel, and link — all from a single declarative script.

> **Status:** Under development (v0.9 ALPHA)

---

## What is ABS?

ABS is a build system that:

- **Tracks source files** and detects when they change (using content hashing, mtime, and file size)
- **Scans `#include` dependencies** to build a dependency graph
- **Invalidates dirty files** — when a header changes, all source files that depend on it (directly or transitively) are marked for recompilation
- **Compiles in parallel** — uses all available CPU cores
- **Caches results** — unchanged files are not recompiled
- **Links** object files into an executable

The build workflow is written in **ABS**, a small scripting language with variables, functions, conditionals, loops, lists, and objects.

---

## Building ABS

```bash
make
```

This compiles the ABS interpreter and runs it with `main.abs` (the default build script).

---

## How It Works

### 1. The Build Script (`main.abs`)

ABS looks for a `.abs` file in the current directory. The default is `main.abs`, or you can pass a script name as an argument:

```bash
./abs mybuild.abs
```

### 2. Tracking Sources

The `track()` function registers source files and detects which ones changed:

```abs
let dirty = track(["src/main.cpp", "src/manager.cpp", "src/operations.cpp"]);
```

`track()` returns a list of files that need recompilation (dirty files). It also scans all `#include` directives to build a dependency graph. If a header changes, `track()` uses **reverse invalidation** to find every source file that depends on it — even transitively.

### 3. Compiling

The `compile()` function compiles source files into object files:

```abs
let objs = compile({
    compiler_path: "g++-15",
    src: ["src/main.cpp", "src/manager.cpp", "src/operations.cpp"],
    tracked_src: dirty,          # files that must be recompiled
    out_dir: "build",            # where .o files go
    flag: "-std=c++20 -I./include"
});
```

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

- `objs` — list of object file paths (from `compile()`)
- `myapp` — name of the output executable
- `g++-15` — compiler/linker to use

`link()` returns the executable path.

---

## The ABS Language

ABS is a small scripting language with:

```abs
# Variables
let x = 5;
const y = "hello";

# Functions
fn add(a, b) {
    return a + b;
}

# Conditionals
if (x > 3) {
    print("x is big");
} else {
    print("x is small");
}

# Loops
for (let i = 0; i < 10; ++i) {
    print(i, " ");
}

while (x > 0) {
    x = x - 1;
}

# Lists
let list = [1, 2, 3];
list.push(4);
print(list.size());

# Objects
let config = {
    compiler_path: "g++-15",
    flag: "-O2"
};
print(config.compiler_path);
```

---

## Native Functions

| Function | Description |
|----------|-------------|
| `track(...)` | Register source files, detect changes, return dirty files |
| `compile(config)` | Compile sources to object files (parallel, cached) |
| `link(objects, name, compiler)` | Link object files into an executable |
| `set_include(path)` | Add an include search path |
| `define(name, value?)` | Define a preprocessor macro |
| `print(...)` | Print values |
| `type(value)` | Get the type of a value |
| `StoN(str)` | Convert string to number |
| `NtoS(num)` | Convert number to string |
| `floor(num)` | Floor a number |
| `max(list)` / `min(list)` | Max/min of a list |
| `system(cmd)` | Run a shell command |
| `timeNow()` | Current time (minutes since midnight) |
| `run(script)` | Run another ABS script |

---

## Caching

ABS stores its state in the `.abs/` directory:

| File | Contents |
|------|----------|
| `.abs/files.cache` | File metadata (content hash, mtime, size) |
| `.abs/dependencies.cache` | Include dependency graph |
| `.abs/objects.cache` | Object file build hashes |

If you change the build system itself or suspect stale cache, delete the `.abs/` directory to start fresh:

```bash
rm -rf .abs
```
