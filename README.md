# Grapher2D
A 2d graphing calculator.

##Features
1. Graphs functions using opengl.
2. Has a 't' variable that mesures the amount of seconds since the function was submited to the grapher and a 'at' variable that mesures the amount of seconds since launch. They can be used to create moving functions.
3. Very very fast.

##Usage
Enter the equations into the textboxes. You can add and take away equation slots using the add graph and remove graph buttons.

##Disclaimer
Must have an OpenGL 3.3+ capable graphics card and drivers

##Compiling
If you dont want to use the prebuilt binaries from the [project website](http://sharhar.ddns.net/projects.html), then you can build the project yourself. To build the project just go into the root folder and use cmake to generate the respective build files for your OS. Then build. It will build all the dependencies and link them automatically. The Executable will be in the bin folder in the cmake build directory.
