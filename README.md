# miniNES

A minimal Famicom/NES emulator built in C++ just for fun.

Compilation
-----------

To compile the native Windows UI open the solution in Visual Studio 2019.

As an alternative **miniNES** comes with a [SDL2](https://www.libsdl.org) UI, that should work on Windows, Linux and macOS. Can be compiled on Windows with the command:

```
g++ -O3 -Wall -std=c++20 machine/*.cpp devices/*.cpp boards/*.cpp ui/ui-sdl.cpp -I. -lmingw32 -lSDL2main -lSDL2 -lopengl32 -o miniNES
```
