# Grapher2D
A 2d graphing calculator.

##Features
1. Graphs functions using opengl.
2. Has a time variable that mesures the amount of seconds since lauch. It can be used to create moving functions.
3. Very very fast.

##Usage
Enter the equations on the editor window <br>
Make sure that arial.ttf is at the working directory of the executable. <br>
It can be downloaded from [here](http://www5.miele.nl/apps/vg/nl/miele/mielea02.nsf/0e87ea0c369c2704c12568ac005c1831/07583f73269e053ac1257274003344e0?OpenDocument).

##Disclaimer
you cant move the cursor using the arrow keys or mouse

##Compiling
If you dont want to use the included binaries you can build the project yourself. To build the project just go into the src folder and use cmake to generate the respective build files for your OS. Then build. It will build all the dependencies and link them automatically. The Executable will be in the bin folder in the cmake build directory.
