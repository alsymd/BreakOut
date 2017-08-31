# BreakOut
A simple breakout demo written in C++14 with SDL2 as framework and scheme(GNU Guile) as the scripting language for customizing and level editing(Currently only one level is supported).

A cleaner version that supports sound effect, graphic level editing is under development. There will likely be less scheme code mixed up with C++ code thus offering higher efficiency and making the codebase easier for maintenance.

## Compilation
$ cd build
$ cmake ..
$ make
The above command lines work under (Arch )linux. Make sure that dependencies(SDL2, GNU Guile 2, compiler that supports c++14) are correctly installed.