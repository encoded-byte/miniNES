# miniNES

A minimal Famicom/NES emulator built in C++ just for fun.

Compilation
-----------

**miniNES** uses [SDL2](https://www.libsdl.org) for output video and audio. Can be compiled on Windows with the command:

```
g++ -O3 -Wall -std=c++20 machine/*.cpp devices/*.cpp boards/*.cpp ui/ui-sdl.cpp -I. -lmingw32 -lSDL2main -lSDL2 -lopengl32 -o miniNES
```
