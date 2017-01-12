# Grapher2D
A 2d graphing calculator.

##Features
1. Graphs functions using opengl.
2. Has a 't' variable that mesures the amount of seconds since the function was submited to the grapher and a 'at' variable that mesures the amount of seconds since launch. They can be used to create moving functions.
3. Very very fast since it uses the GPU to do all the calculations

##Usage
Enter the equations into the textboxes and press the enter key to graph them. You can add and take away equation slots using the add graph and "X" buttons.

##Disclaimer
Must have an OpenGL 4.2 and GLSL 4.20 capable graphics card and drivers<br>
Sadly Mac's do not support OpenGL 4.2 because Apple refuses to release drivers for it. If you are on a Mac and what to run the graphing calculator then you can clone [this](https://github.com/sharhar/Grapher2D/tree/80e7226c20bc0135757d6028cd65014dab6007dd) previous version of the graphing calculator. It will be much slower because it won't use the GPU to generate function values, but it will do everything that the new version can do. I might create a Mac branch later for people who don't have OpenGL 4.2.

##Compiling
If you dont want to use the prebuilt binaries from the [project website](http://sharhar.github.io/projects.html), then you can build the project yourself. To build the project use cmake to generate the respective build files for your OS (internet connection required to download all the dependecies). Then build. It will build all the dependencies and link them automatically. The Executable will be in the bin folder in the cmake build directory.

##Dependencies
All dependencies are downloaded by Cmake when build files are generated.<br>
This project uses a library that I made called GLUI for window management and UI rendering. <br>
The dependecies of this library are:
* [GLFW] (http://www.glfw.org/) 3.2
* [FreeType] (https://www.freetype.org/) 2.6
* [glcorearb.h] (https://www.opengl.org/registry/api/GL/glcorearb.h)
