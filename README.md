wxChaos
========
An open source fractal generator extensible with scripts. wxChaos aims to be relatively simple and yet providing the user with many predefined fractals and algorithms to create beautiful results in less time. It also allows the user to create new fractals with user formulas and user scripts.

Features
========
* Many fractals and algorithms preimplemented.
* Quick preview of Julia fractals.
* Zoom recorder.
* User formulas.
* Fractal dimension calculator.
* Create any types of fractals through an scripting language.

Download
========
* [Windows portable app](https://github.com/CarlosManuelRodr/wxChaos/releases/tag/v1.2.0-beta)

Screenshot
==========
Main window

![Screenshot](images/screenshot.png?raw=true "Screenshot")

Script editor

![ScriptEditor](images/ScriptEditor.png?raw=true "ScriptEditor")

Fractal dimension calculator
![Dimension](images/Dimension.png?raw=true "Dimension")

Zoom recorder
![ZoomRecorder](images/ZoomRecorder.png?raw=true "ZoomRecorder")


Showcase
========
Examples of images generated with wxChaos.

![Example1](images/example1.jpg?raw=true "Example 1")
![Example2](images/example2.jpg?raw=true "Example 2")
![Example3](images/example3.jpg?raw=true "Example 3")
![Example4](images/example4.jpg?raw=true "Example 4")

Build Instructions
==================  

### 📦 Supported Build Types
There are two ways to build wxChaos:

#### 1. **Visual Studio Solution**
Open `wxChaos.sln` with Visual Studio and build the solution for `x86` (32-bit). Only `x86` builds are supported.

#### 2. **CMake Build (Recommended for CLI/Ninja)**
The CMake project is configured to use `wxWidgets` through [vcpkg](https://github.com/microsoft/vcpkg).

##### Steps:
1. Clone and bootstrap vcpkg:
   ```bash
   git clone https://github.com/microsoft/vcpkg.git
   cd vcpkg
   bootstrap-vcpkg.bat
   
2. Install wxWidgets (x86):
   ```bash
   vcpkg install wxwidgets:x86-windows

3. Configure your project with CMake:
   ```bash
   cmake -S . -B build ^
   DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake ^
   -DVCPKG_TARGET_TRIPLET=x86-windows ^
   -G Ninja

4. Build:
   ```bash
   cmake --build build


🛑 Only the x86-windows triplet is supported. Make sure your compiler matches the architecture. If you're using CLion, you can specify the above options in the CMake Profile settings.

License
========
wxChaos is licensed under the GNU General Public License version 3.0 (GPLv3).

Bugs
========
All bugs must be reported at:
https://github.com/CarlosManuelRodr/wxChaos/issues
