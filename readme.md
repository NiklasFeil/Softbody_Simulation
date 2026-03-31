Vorlesung: Einführung in die Computerphysik
Projekt: Softbody-Simulation : Mass-Spring System vs Extended Position Based Dynamics
Gruppenmitglieder: Niklas Feil

Contols: 
WASD - Move camera around center
QE - Zoom in and out
RMB - Drag vertices around

Dependencies:
Project uses GLFW, glad, Eigen, glm, OpenGL.
Dependencies required for the above mentioned libraries / APIs are also required to run this project.

Build:
I supplied a docker file, but I could not get the container to work with GLFW. However, it can be taken as an inspiration to build the project. Premake5 is used to build the project.
The executable must be ran from inside the build folder, otherwise ressources such as models and shaders cannot be found by the application.
There is also a ./build.sh in this project, which can be used as reference to build the project.