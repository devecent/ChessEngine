# ChessEngine

C++20 chess engine with a UCI interface.

## Build and test

Requires CMake 3.20 or newer and a C++20 compiler.

On Windows with MSYS2 UCRT64/MinGW, put its `bin` directory on PATH so CMake can
find `g++` and `mingw32-make`. From the project root in PowerShell:

```powershell
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

For Visual Studio, use a separate build directory:

```powershell
cmake -S . -B build-vs
cmake --build build-vs --config Release --parallel
ctest --test-dir build-vs -C Release --output-on-failure
```

Run the MinGW executable from its output directory so it finds the copied
opening book:

```powershell
Push-Location build
.\devecent.exe
Pop-Location
```

For Visual Studio, use `build-vs/Release` instead. The engine reads UCI commands
from standard input; enter `uci`, `isready`, or `quit` to check it manually.

Use `-DCMAKE_BUILD_TYPE=Debug` in a separate MinGW build directory for debugging.
To build only the engine, configure with `-DBUILD_TESTING=OFF`.

Regression coverage and direct compiler instructions are in
[tests/README-regression.md](tests/README-regression.md).
