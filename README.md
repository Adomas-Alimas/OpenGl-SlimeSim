# OpenGl Slime Simulation
A [physarum slime mold](https://en.wikipedia.org/wiki/Physarum_polycephalum) simulation that makes use of modern OpenGL compute shaders and other features.

## Inspirations
Originally inspired by a video of [SebLague's](https://github.com/SebLague) project [Slime-Simulation](https://github.com/SebLague/Slime-Simulation), this is a very similar project, but made in C++ and OpenGl instead of C# and DirectX.

Additional credit goes to Jeff Jones, who, I believe, inspired most of these projects with his article (available [here](https://uwe-repository.worktribe.com/output/980579)) on pattern formation and evolution in physarum simulations.

Also Sage Jenson's variation of this project and it's helpful write up, which can be found [here](https://sagejenson.com/physarum).

## Project file breakdown
- [lib](lib) - contains all of the dependencies for glfw and glad.
- [presets](presets) - contains different value presets for different program output.
- [shaders](shaders) - contains GLSL code shaders that are used at run-time.
- [glfw3.dll](glfw3.dll) - dynamically linked lib that is required at run-time.
- [json.hpp](json.hpp), [shader.h](shader.h) - other dependencies.
- [main.cpp](main.cpp) - main code file.

## Compiling this project
If you don't want to compile this project yourself pre-compiled binaries can be found in releases.

**Requirements:**
- [lib](lib) folder with all of it's contents in compile directory.
- [glfw3.dll](glfw3.dll), [json.hpp](json.hpp), [main.cpp](main.cpp) and [shader.h](shader.h) file in compile directory.
- Preferred compiler.

I have tested out compiling this project using g++ in a Windows 10 environment, if you are on a similar environment you can use a simple one-liner:
```
g++ main.cpp lib/glad/src/glad.c lib/glfw-WIN32/lib-mingw-w64/libglfw3dll.a -Ilib/glfw-WIN32/include -Ilib/glad/include -o main.exe
```

If you are not using g++ you will have to compile code according to your compilers rules, most importantly you have to link against ```lib/glad/src/glad.c``` and ```lib/glfw-WIN32/lib-mingw-w64/libglfw3dll.a```, and get their respective include paths right, which are ```lib/glfw-WIN32/include``` and ```lib/glad/include```.

## Running this project

If you have successfully compiled this project or have downloaded the .exe file you can try running it.

**Requirements:**
- main.exe or differently named main executable file.
- [presets](presets) folder with at least 1 preset.
- [shaders](folder) with all of it's contents.
- [glfw3.dll](glfw3.dll) file.

**Technical requirements:**
- A somewhat recent GPU that can support OpenGL 4.5 core version or later.
- Depending on simulation size, enough ram so that the particles can be loaded.

All of these files should be in the same directory as the executable. To run simply run the ```main.exe``` or differently named executable and follow the instructions in console.

**Presets and their use:**

Preset files are simple ```json``` files, which can be found in [presets](presets) folder. They consist of settings, which are used for simulation execution. When you successfully run the executable it will ask for which preset you want to use. I recommend using the defaults for first few runs, then, I encourage you to try changing some values and seeing what changes

**Settings:**
- agentNumber - as the name suggests it's how many individual "agents" or particles are in a simulation, keep in mind that each agent takes up 12 bytes of memory so don't make the simulations too big, otherwise it might crash your pc.
- moveSpeed - each agent movement speed which is used in their movement calculations.
- turnSpeed - how quickly agents turn towards trails.
- sensorAngle - angle between 3 sensors in front of each agent that detect trail strength.
- sensorDistance - how far away the sensors are from the agent.
- mapWidth - screen space width in pixels.
- mapHeight - screen space height in pixels.
- decayRate - how quickly the trails decay, should be between 0 and 1.
- diffuseRate - how quickly the trails diffuse with environment, should be between 0 and 1.
- spawnMethod - how the agents are spawned at the start of simulation. Choices are: ```centre```, ```circle```, ```random```.
- simulationShader - should always be ```stageFinal```