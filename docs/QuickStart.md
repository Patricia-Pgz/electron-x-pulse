### Quick Start: ElectronXPulse

This is a simple quick start guide how to clone my project and get it running.
Prequisits: C++20 Compiler (MSVC) & Windows11 SDK installed

1. You can clone the code with this command:

```bash
git clone --recursive --depth 1 https://gitlab2.informatik.uni-wuerzburg.de/GE/Teaching/gl3/projects/2024/29-gl3-pongratz.git
```

2. Once cloned, you can open the root folder in CLion. And load the `CMAKEList.txt` file from the "code" folder.

3. In CLion go to Settings->Build,Execution,Deployment->Toolchanins add the Microsoft Visual Studio Compiler (2019 or
   2022) and hit Apply

4. You should now have a running configuration -> compile the code and play ElectronXPulse with integrated Level Editor!

### Example Usage of Electrine in your game:

To use Electrine Engine in your own game, clone as mentioned above and add the engine as subdirectory and link your game
executable against Electrine in your CMAKEList.

In your CMakeList:

```cmake
cmake_minimum_required(VERSION 3.18)
project(MyNewGame)

// Add the Electrine Engine directory (adjust the path as needed)
add_subdirectory(path/to/Electrine Electrine_build)

add_executable(MyNewGame main.cpp)

// Link your game executable with the Electrine Engine library
target_link_libraries(MyNewGame PRIVATE Electrine)

// Enable C++20 features to match the engine's requirements
target_compile_features(MyNewGame PRIVATE cxx_std_20)
```

Include headers in your .cpp files:

```cpp
#include <Electrine/SomeHeader.h>
```