wxChaos
========
An open source fractal generator extensible with scripts. wxChaos aims to be relatively simple and yet providing the user with many predefined fractals and algorithms to create beautiful results in less time. It also allows the user to create new fractals with user formulas and user scripts.

Features
========
* Many fractals and algorithms preimplemented.
* Quick preview of Julia fractals.
* User formulas.
* Create any types of fractals through an scripting language.

Download
========
* [Windows Installer](https://github.com/cmrm/wxChaos/releases/download/v1.0.3/wxChaos-1.0.3.exe)

Screenshot
==========
![Screenshot](images/screenshot.png?raw=true "Screenshot")

![ScriptEditor](images/ScriptEditor.png?raw=true "ScriptEditor")

![Dimension](images/Dimension.png?raw=true "Dimension")

![ZoomRecorder](images/ZoomRecorder.png?raw=true "ZoomRecorder")


Showcase
========
Examples of images generated with wxChaos.

![Example1](images/example1.jpg?raw=true "Example 1")
![Example2](images/example2.jpg?raw=true "Example 2")
![Example3](images/example3.jpg?raw=true "Example 3")
![Example4](images/example4.jpg?raw=true "Example 4")

Build Instructions
========

Linux
--------

To build you'll need the following libraries:

* [wxGtk](https://www.wxwidgets.org/)
* [SFML](http://www.sfml-dev.org/) (Version 1.6)

On Debian based distros: "sudo apt-get install libwxgtk2.8-dev  libsfml-dev"

To install use make && make install.
If you want to use the app without installing it you can build the no_setup version with "make no_setup".
You can build a package with the "build-pkg" script.

Windows
--------
You'll need to have installed WxWidgets on your system first.
If you want to make the setup version use the Unicode Setup build and use the NSIS script provided.

To compile use the MSVC++ project file.

Warning
=======
This project was developed when I was still learning to code. The code is messy, ugly and uses outdated libraries. Please consider using another well mantained fractal program.

Copyright
========
wxChaos is licensed under the GNU General Public License version 3.0 (GPLv3).

Bugs
========
All bugs must be reported at:
https://github.com/cmrm/wxChaos/issues
