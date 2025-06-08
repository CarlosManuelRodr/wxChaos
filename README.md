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

License
========
wxChaos is licensed under the GNU General Public License version 3.0 (GPLv3).

Bugs
========
All bugs must be reported at:
https://github.com/CarlosManuelRodr/wxChaos/issues

Build
=====
This project now uses [vcpkg](https://github.com/microsoft/vcpkg) and CMake. Dependencies are automatically resolved (SFML 2.6.2 and wxWidgets).

```bash
# bootstrap vcpkg
git clone https://github.com/microsoft/vcpkg.git
./vcpkg/bootstrap-vcpkg.sh

# install dependencies
./vcpkg/vcpkg install

# configure and build
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build
```
