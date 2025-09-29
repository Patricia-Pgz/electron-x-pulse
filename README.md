This repository includes a c++, OpenGL-based 2D side-scrolling platformer game with a level editor and an engine
framework to quickly create similar sidescrollers.

![](images/Cover.png)

# GameLab Project Repository

| General Info       |                                                                          |
|--------------------|--------------------------------------------------------------------------|
| Working Title      | ElectronXPulse                                                           |
| Final Title        | Electrine Engine                                                         |
| Target Platform(s) | Windows                                                                  |
| Start Date         | 01.11.2024                                                               |
| Study Program      | Games Engineering B.Sc.                                                  |
| Engine Version     | Electrine 0.0.1                                                            |

## Abstract

Electrine Engine transforms rhythm into gameplay. Built for flexibility and creative control, it powers core systems
like rendering, physics, audio, UI, state management, and ECS.
With its integrated level editor, level loading system, and audio analysis, developers can intuitively design levels that
precisely align with the beat.
As a showcase, the included 2D platformer ElectronXPulse challenges players to jump, react, and survive in a world
driven by high-energy EDM tracks. Designed to sharpen reflexes and timing skills, every interaction is coupled to the
beat, delivering a tightly crafted, rhythm-based experience.

## Repository Usage Guides

```
RepositoryRoot/
    ├── README.md           
    ├── builds/                 // Archives (.zip) of built executables of the game
    ├── code/
    │   ├── assets/             // Game assets: audio tracks, levels, shaders, textures, UI assets: ui textures, fonts 
    │   ├── engine/             // Engine code
    │   ├── extern/             // External dependencies/libraries         
    │   ├── game/               // Game code
    │   └── CMakeLists.txt      // Project root CMakeList
    ├── docs/                   // Doxygen files
    ├── documentation/          // API Docs & Handbook/Manual (PDF)
    ├── poster/                 // Poster (PDF)
    ├── report/                 // Report (PDF)
    └── trailer/                // Trailer (MP4)
```

### Quick Start: ElectronXPulse

This is a simple quick start guide on how to clone my project and get it running.

**Prerequisites:** C++20 Compiler (MSVC) & Windows11 SDK installed.  
(e.g. Windows 11 SDK (10.0.26100.0) MSVC v143 - VS 2022 C++-x64/x86-Buildtools)

1. You can clone the code with this command:  
   <span style="font-size: 90%; color: gray;">Note: This clones the project with all needed external submodules.
   See the [External Libraries section](#libs) for details.</span>
   ```bash
   git clone --recursive --depth 1 https://github.com/Patricia-Pgz/electron-x-pulse.git
   ```

2. Once cloned, you can open the root folder in CLion. And load the `CMAKEList.txt` file from the "code" folder.

3. In CLion, go to Settings->Build,Execution,Deployment->Toolchains add the Microsoft Visual Studio Compiler (2019 or 2022), and hit Apply

4. You should now have a running configuration -> compile the code and play ElectronXPulse with integrated Level Editor!

### Example Usage of Electrine in your game:

To use Electrine Engine in your own game, clone as mentioned above, and add the engine as subdirectory and link your game
executable against Electrine in your CMAKEList.

In your CMakeList:

```cmake
cmake_minimum_required(VERSION 3.18)
project(MyNewGame)

# Add the Electrine Engine directory (adjust the path as needed)
add_subdirectory(path/to/Electrine Electrine_build)

add_executable(MyNewGame main.cpp)

# Link your game executable with the Electrine Engine library
target_link_libraries(MyNewGame PRIVATE Electrine)

# Enable C++20 features to match the engine's requirements
target_compile_features(MyNewGame PRIVATE cxx_std_20)
```

Include headers in your .cpp files:

```cpp
#include <Electrine/SomeHeader.h>
```

## <a name="libs"></a>📂 External Libraries (Git Submodules)

This project uses external libraries as Git submodules:

- [`extern/aubio`](https://github.com/aubio/aubio) — Audio analysis (e.g. beat, onset detection)
- [`extern/box2d`](https://github.com/erincatto/box2d) — 2D physics engine
- [`extern/entt`](https://github.com/skypjack/entt) — Entity Component System (ECS) framework
- [`extern/glad`](https://github.com/Dav1dde/glad) — OpenGL function loader
- [`extern/glfw`](https://github.com/glfw/glfw) — OpenGL window and input handling
- [`extern/glm`](https://github.com/g-truc/glm) — OpenGL Mathematics
- [`extern/glaze`](https://github.com/stephenberry/glaze) — de-/serialization library, used for level saving/loading
- [`extern/json`](https://github.com/nlohmann/json) — JSON library for level saving/loading
- [`extern/imgui`](https://github.com/ocornut/imgui) — create minimal game/editor GUI
- [`extern/soloud`](https://github.com/jarikomppa/soloud) — Audio playback
- [`extern/stb`](https://github.com/nothings/stb) — used stb image for texture loading

Royaltyfree textures were taken from [CraftPix](https://craftpix.net/), [itch.io](https://itch.io/), [Freepik](https://freepik.com/), generated with [Sora](https://sora.chatgpt.com/) and adjusted by myself. 
Royaltefree soundtracks were taken from [fesliyanSTUDIOS](https://www.fesliyanstudios.com/) and [pixabay](https://pixabay.com/).

