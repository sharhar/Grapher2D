# Grapher2D
A 2d graphing calculator.

##Features
1. Graphs functions using opengl.
2. Has a time variable that mesures the amount of seconds since lauch. It can be used to create moving functions.
3. Very very fast.

##Usage
CLI instructions:<br>
(./ if on linux/mac)Grapher2D(.exe if on windows) [equations to graph] <br>
<br>
example on windows:   (Grapher2D.exe "x^2" "sin(x)" "sec(x - time)")<br>
or if on linux/mac:   (./Grapher2D "x^2" "sin(x)" "sec(x - time)")<br>

##Compiling
If you dont want to use the included binaries you can build the project yourself. To build the project just go into the src folder and use cmake to generate the respective build files for your OS. Then build. It will build SDL and link it automatically.
