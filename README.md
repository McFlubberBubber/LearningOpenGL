# LearningOpenGL - rendering a 'scene'


## Introduction

This is a repository that consists of my first exposure to the world of graphics programming. This project utilises the guidance that is offered by [learnopengl.com](https://learnopengl.com) and uses **OpenGL 4.6.0**.

This is NOT a project that aims to create a functional game engine **yet**. Instead, this project is solely to play around with the features that OpenGL offers, figuring out how shaders work, rendering things to a screen, etc. It also is a way to practice my application architecture skills and learn C++ and GLSL!

![Rendering the main scene with the console being drawn](media/main_scene_and_console.png)
**(Picture taken as of 1st January, 2026 - Main scene with console being drawn)**

![Example of instance rendering a planet with asteroids](media/space_scene.png)
**(Picture taken as of 16th October, 2025 - Space scene with instance rendering)**

## Installation

- You can clone the whole project by just downloading the zip file here on **GitHub** (the green button with <>Code).

- Alternatively, you can clone the repository with the terminal with the following command:

```
git clone https://github.com/McFlubberBubber/LearningOpenGL.git
```

The project currently **does not** use CMake and instead was built using Visual Studio 2022's solution stuff. This may change in the future, but for now there isn't any build system in place (therefore some compatibility issues may occur if you're doing this on macOS or Linux, sorry about that).

### Usage

There should not be any linker errors (hopefully!), all include / libraries directories are set up relative to the solution directory (not the project). Therefore it should be able to run just fine on x64 debug and release builds.

## Controls

- Movement Keys = WASD
- Sprint = Left shift
- Change FOV = Mouse scroll
- Toggle camera movement = E
- Cycle post-processing effects = Up / Down arrow keys
- Toggle debug mode = Q
- Open / close menu = Escape
- Navigate menu options = Up / Down arrow keys

The menu has as "options" page that can be used to toggle some features in the application (fullscreen toggles, v-sync, etc.) and switching scenes!

## Dependencies used

GLAD, GLFW, GLM, Assimp, Freetype, stb_image.h


#### Special thanks
1. [@tokyospliff](https://www.youtube.com/@tokyospliff) for inspiring this whole thing
2. [@TheCherno](https://www.youtube.com/@TheCherno) for his C++ / OpenGL videos
3. [@WindersCharlie / @EQUAL](https://github.com/cw1169) for being a bud who is also working on his own OpenGL stuff with me
