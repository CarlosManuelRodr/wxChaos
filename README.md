wxChaos build instructions.

===  Linux  ===
To build you'll need the following packages:

- libwxgtk2.8-dev
- libglu1-mesa-dev
- libsfml-dev
- libgtk2.0-dev

On Debian based distros: "sudo apt-get install libwxgtk2.8-dev libglu1-mesa-dev libsfml-dev libgtk2.0-dev"

To install use make && make install.
If you want to use the app without installing it you can build the no_setup version with "make no_setup".
You can build a package with the "build-pkg" script.


===  Windows ===
You'll need to have installed WxWidgets on your system first.
If you want to make the setup version use the Unicode Setup build and use the NSIS script provided.

To compile use the MSVC++ project file.
