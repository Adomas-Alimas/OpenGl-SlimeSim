# OpenGl Slime Simulation

A [physarum slime mold](https://en.wikipedia.org/wiki/Physarum_polycephalum) simulation written in C++, that makes use of modern (core 4.5+) OpenGL features.


![image](https://github.com/Adomas-Alimas/OpenGl-SlimeSim/assets/39675293/f0bbd164-1a70-4af2-b1d4-c2d534679b9a)

## Inspirations

Originally inspired by a video of [SebLague's](https://github.com/SebLague) project [Slime-Simulation](https://github.com/SebLague/Slime-Simulation), this is a very similar project that substitutes C# and DirectX for C++ and OpenGl.

Additional credit goes to Jeff Jones who I think inspired most of these projects with his article (available [here](https://uwe-repository.worktribe.com/output/980579)) on pattern formation and evolution in physarum simulations.

Also Sage Jenson's variation of this project and it's helpful write up which can be found [here](https://sagejenson.com/physarum).

## Project file breakdown

- [lib](lib) - contains all of the dependencies for compiling `main.cpp`.
- [presets](presets) - contains different value presets for different program output.
- [shaders](shaders) - contains GLSL code shaders that are used at run-time.
- [glfw3.dll](glfw3.dll) - dynamically linked lib that is required at run-time.
- [main.cpp](main.cpp) - main code file.

## Compiling this project

**Requirements:**

- [lib](lib) folder with all of it's contents in compile directory.
- [glfw3.dll](glfw3.dll) and [main.cpp](main.cpp) files in compile directory.
- Preferred compiler.

I have tested out compiling this project using g++ in a Windows 10 environment, if you are on a similar environment you can use a simple one-liner from the `main.cpp` file directory:

```shell
g++ main.cpp lib/glad/src/glad.c lib/glfw-WIN32/lib-mingw-w64/libglfw3dll.a -Ilib/glfw-WIN32/include -Ilib/glad/include -o main.exe
```

If you are not using g++ you will have to compile the code according to your compilers specs. When compiling you have to link against `lib/glad/src/glad.c` and `lib/glfw-WIN32/lib-mingw-w64/libglfw3dll.a` and get their respective include paths right. These are `lib/glfw-WIN32/include` and `lib/glad/include`.

## Running this project

If you have successfully compiled this project or have downloaded the .exe file you can try running it.
Use the following command for it:

```shell
./main.exe [presetName]
```

`presetName` being the preset file title without the `.json` file extension.

**Technical requirements:**

- A somewhat recent GPU that can support OpenGL 4.5 core version or later.
- Depending on simulation size enough ram so that the particles can be loaded (Shouldn't be a problem for simulations with `<50,000,000` particles).

**Requirements:**

- [main.exe](main.exe) or differently named main executable file.
- [presets](presets) folder with at least 1 preset.
- [shaders](shaders) folder with all of it's contents.
- [glfw3.dll](glfw3.dll) file.

All of these files should be in the same directory as the executable.

## Presets and their use

Preset files are simple `.json` files which can be found in the [presets](presets) folder. They store variable settings that are used for simulation execution. When running the program the only required argument is preset file name. I recommend using the defaults for the first few runs, after that try  changing some values and see what changes.

**Settings:**

- agentNumber **[num]** - as the name suggests it's how many individual "agents" or particles are in a simulation, keep in mind that each agent takes up 12 bytes of memory in initial loading period, after loading agents their inforamtion is stored on the GPU.
- moveSpeed **[num]** - each agent movement speed which is used in their movement calculations.
- turnSpeed **[radians]** - how quickly agents turn towards trails.
- sensorAngle **[radians]** - angle between 3 sensors in front of each agent that detect trail strength.
- sensorDistance **[num]** - how far away the sensors are from the agent.
- mapWidth **[num]** - screen space width in pixels.
- mapHeight **[num]** - screen space height in pixels.
- decayRate **[num]** - how quickly the trails decay, should be between 0 and 1.
- diffuseRate **[num]** - how quickly the trails diffuse with environment, should be between 0 and 1.
- spawnMethod **[string]** - how the agents are spawned at the start of simulation. Choices are: `centre`, `circle`, `random`.
- simulationShader - should always be `stageFinal`




![image](https://github.com/Adomas-Alimas/OpenGl-SlimeSim/assets/39675293/9e332579-d995-43bc-8fe3-ff74115589ca)
