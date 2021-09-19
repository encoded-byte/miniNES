# miniNES

A minimal Famicom/NES emulator written in C++ just for fun.

Compilation
-----------

**miniNES** uses [SDL2](https://www.libsdl.org) for output video and audio. Can be compiled on Windows with the command:

```
g++ -Wall -std=c++20 machine/*.cpp devices/*.cpp boards/*.cpp miniNES.cpp -I. -lmingw32 -lSDL2main -lSDL2 -lopengl32 -o miniNES
```