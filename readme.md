WhirlWindWarp Screensaver for Windows
=====================================

# Summary
- [Description](#description)
- [Compilation](#compilation-requirements)
- [Install](#install)
- [Screenshots](#screenshots)
- [Repository information](#repository-information)

# Description
Windows port of the Linux screensaver WhirlWindWarp using Qt 5.x. Some improvements over the original version:
- Antialiasing
- Configurable particle trails: on/off, fade, variable length.
- Frames per second configuration. 

In the future maybe a port to OpenGL will be provided to remove the Qt dependency.

# Compilation requirements
## To build the tool:
* cross-platform build system: [CMake](http://www.cmake.org/cmake/resources/software.html).
* compiler: [Mingw64](http://sourceforge.net/projects/mingw-w64/) on Windows.

## External dependencies
The following libraries are required:
* [Qt 5 opensource framework](http://www.qt.io/).

# Install
The only current option is build from source as binaries are not provided. 

# Screenshots
Just a bunch of particles dancing around...

![screensaver]()

Configuration dialog.

![configuration]()

# Repository information

**Version**: 1.0.0

**Status**: finished

**cloc statistics**

| Language                     |files          |blank        |comment           |code  |
|:-----------------------------|--------------:|------------:|-----------------:|-----:|
| C++                          |   5           | 133         |   168            |  528 |
| C/C++ Header                 |   4           |  54         |   151            |  116 |
| CMake                        |   1           |  15         |    10            |   54 |
| **Total**                    | **10**        | **202**     | **329**          | **698** |
