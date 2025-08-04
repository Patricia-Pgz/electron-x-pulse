### Quick Start: ElectronXPulse

This is a simple quick start guide how to clone my project and get the game running.
For tips on gameplay or the level editor, refer to [How to play ElectronXPulse](wiki/game.md).

**Prerequisites:** C++20 Compiler (MSVC) & Windows11 SDK installed.  
(e.g. Windows 11 SDK (10.0.26100.0) & MSVC v143 - VS 2022 C++-x64/x86-Buildtools)

1. You can clone the code with this command:  
   <span style="font-size: 90%; color: gray;">Note: This clones the project with all needed external submodules.
   See the External Libraries section in the Manual for details.</span>
   ```bash
   git clone --recursive --depth 1 https://gitlab2.informatik.uni-wuerzburg.de/GE/Teaching/gl3/projects/2024/29-gl3-pongratz.git
   ```
2. Once cloned, you can open the root folder in CLion. And load the `CMAKEList.txt` file from the "code" folder.

3. In CLion go to Settings->Build,Execution,Deployment->Toolchains add the Microsoft Visual Studio Compiler (2019 or
    2022) and hit Apply

4. You should now have a running configuration -> compile the code and play ElectronXPulse with integrated level editor!
