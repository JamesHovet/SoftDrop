# SoftDrop
[WIP] Soft Drop is a NES emulator written in C++ as a fun side project

![Tetris Screenshot](https://jameshovet.github.io/images/SoftDropTetrisScreenshot.png)

## Build instructions (macOS)
- install these dependencies, probably using brew: `cmake`, `sdl2`, `glew`, `boost` (or alternatively, just boost filesystem and program_options). 
- clone this repo and `cd` into it. 
- exectue these commands, one after another
```
cd build
cmake .
make -j
```


To play a ROM (only Mapper 000 and 001 Rom's are supported) run:
```
./SoftDrop --play <path/to/rom>
```
