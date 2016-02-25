# CHIP-Towers
A pet-project CHIP-8 Interpreter. Done in parallel to learning SDL and C++ with CMake, so please be patient.

## Contents:
- `cmake`: CMake folder with dependencies, namely the files to find SDL2.
- `emulator`: The Emulator project itself, with a CMakeList.txt to handle dependencies.
- `tests`: Various unit tests. THESE DO NOT COMPILE, because they test the emulator's internal state (private fields), and also Catch has conflicts with SDL. They were used before adding SDL support, and all passed.

## Compilation and Execution:

The emulator uses CMake out-of-source compilation:
```
  > mkdir build<environment>
  > cd build<environment>
  > cmake -G "<environment_flag" ..
  > cmake --build emulator
  ```
  NOTE for Windows users: In order for CMake to be able to find SDL2, you need to define an environment variable [MORE INFO HERE](http://www.willusher.io/sdl2%20tutorials/2014/03/06/lesson-0-cmake/).
  
To run one of the ROMs included in emulator/res/games, copy the ROM to your `build<...>` folder, and run 
```
  > emulator\emulator.exe [GAME]
  ```
  
## Controls:

  The CHIP-8 has a 16-key keypad, which in the simulator is linked to the following keys:
  
| 1 | 2 | 3 | 4 |
|---|---|---|---|
| Q | W | E | R |
| A | S | D | F |
| Z | X | C | V |
  