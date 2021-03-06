# Wormhole-X-treme
A space flight simulator in an automatically generated wormhole.<br />
Developed to demonstrate OpenGL capabilities of the [Qt](https://www.qt.io/) toolkit.<br />
GUI elements as well as all graphics and 3D rendering are all powered by Qt.<br />

Written in C++ (Qt), utilizes OpenGL.
![Wormhole-X-treme screenshot](https://smejkal.software/img/wormhole_scr2.jpg)

## Installation
Installer works natively in Windows and with `wine` in Linux.<br />
Alternatively you can compile the code for your OS with `g++` ([GCC](https://gcc.gnu.org/)).

## Controls
```
P          - Play / Pause
R          - Reset
F          - Fullscreen on / off
Arrow Keys - Steering
W,S,A,D    - Steering
Spacebar   - Turbo
Mouse      - Camera controls (LMB x and y axis, RMB z axis)
```

During the `Pause` mode camera switches to free mode.<br />
During the `Play` mode camera binds back to the ship.

## Classes
```
cDSettings   - Wrapper for settings.ui, created by Qt Designer
cGLObject    - Basic model for every openGL object in scene (wormhole, ufo, etc.)
cGLWidget    - OpenGL widget, heart of the application. Calculations, painting, etc
cMainWindow  - Base window contains opengl widget and GUI
cObj2OGL     - Obj file parser
cUfo         - Unidentified Flying Object
cWormhole    - Unpredictably curved "tube". Object of high importance in application
vec3         - Auxiliary class for vector computations
```

## Documentation
Open `doc/index.html` with a browser.<br />
`doc` was generated from code using [Doxygen](https://www.doxygen.nl/index.html)
