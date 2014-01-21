wxChaos
========
An open source fractal generator extensible with scripts. wxChaos aims to be relatively simple and yet providing the user with many predefined fractals and algorithms to create beautiful results in less time. It also allows the user to create new fractals with user formulas and user scripts.

Features
========
* Many fractals and algorithms preimplemented.
* Quick preview of Julia fractals.
* User formulas.
* Create any types of fractals through an scripting language.


Install
========
On Linux you can build it yourself or use the following Ubuntu PPA: "ppa:fis-carlosmanuel/wxchaos".
The installer for Windows can be found at: "http://sourceforge.net/projects/grufifractalgen/"

Build Instructions
========

Linux
--------

To build you'll need the following packages:

- libwxgtk2.8-dev
- libglu1-mesa-dev
- libsfml-dev
- libgtk2.0-dev

On Debian based distros: "sudo apt-get install libwxgtk2.8-dev libglu1-mesa-dev libsfml-dev libgtk2.0-dev"

To install use make && make install.
If you want to use the app without installing it you can build the no_setup version with "make no_setup".
You can build a package with the "build-pkg" script.

Windows
--------
You'll need to have installed WxWidgets on your system first.
If you want to make the setup version use the Unicode Setup build and use the NSIS script provided.

To compile use the MSVC++ project file.

Copyright
========
wxChaos is licensed under the GNU General Public License version 3.0 (GPLv3).

Bugs
========
All bugs must be reported at:
https://github.com/cmrm/wxChaos/issues
