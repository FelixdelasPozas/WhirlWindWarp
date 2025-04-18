WhirlWindWarp Screensaver for Windows
=====================================

# Summary
- [Description](#description)
- [Compilation](#compilation-requirements)
- [Install](#install)
- [Screenshots](#screenshots)
- [Repository information](#repository-information)

# Description
Windows port of the Linux screensaver WhirlWindWarp using OpenGL, with some improvements over the original version.
It detects and uses all available monitors (see screenshots).

If you like it you can support me on [ko-fi](https://ko-fi.com/felixdelaspozas)! Your support is much appreciated!!

## Configuration

The configuration dialog can be accessed by right-clicking the .src file and selecting "Configure" option, or by the Windows screensaver selection dialog. The options are: 
- Particle size: initial size for particles. 
- Particle density: low, medium and high.
- Antialiasing: on/off.
- Particle trails: on/off.
- Motion blur: on/off.

# Compilation requirements
## To build the screensaver:
* cross-platform build system: [CMake](http://www.cmake.org/cmake/resources/software.html).
* compiler: [Mingw64](http://sourceforge.net/projects/mingw-w64/) on Windows.

## External dependencies
The following libraries are required:
* [GLFW library](https://www.glfw.org/).

# Install
Download the [latest release](https://github.com/FelixdelasPozas/WhirlWindWarp/releases) and decompress the contents in the C:\Windows\System32 directory, then it will be available to configure and select from the Windows screensaver selection dialog.

The older version, built using Qt5 can be downloaded [here](https://github.com/FelixdelasPozas/WhirlWindWarp/releases/tag/1.0.0). The installation process is the same.

# Screenshots
Just a bunch of particles dancing around in two monitors...

![Image1](https://github.com/user-attachments/assets/654d66d2-5df8-40ce-9580-33a96d9fe193)
![Image2](https://github.com/user-attachments/assets/239c8a44-bf6f-4e9f-aa79-404c8666dcfc)
![Image3](https://github.com/user-attachments/assets/2fe9e9ae-2c09-47a8-9697-0282531face2)
![Image4](https://github.com/user-attachments/assets/6b29984d-1f93-4fbf-93ee-32be6d5a71ae)

Configuration dialog.

![optionsdialog](https://github.com/user-attachments/assets/de2749ec-65c4-4d4e-bc57-16632c359a68)

# Repository information

**Version**: 2.0.0

**Status**: finished

**cloc statistics**

| Language                     |files          |blank        |comment           |code      |
|:-----------------------------|--------------:|------------:|-----------------:|---------:|
| C++                          |   5           | 244         |   194            |  996     |
| C/C++ Header                 |   6           | 117         |   251            |  348     |
| CMake                        |   1           |   9         |     3            |   41     |
| **Total**                    | **12**        | **370**     | **448**          | **1385** |
