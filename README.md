# Eclipse

Eclipse is a 2D game engine written in C++.

## What is included

- SDL2 for the window, input, audio, and timing
- OpenGL for drawing
- An editor with ImGui
- Sprite, shader, texture, and audio support
- Tilemap loading from Tiled
- Basic entities, movement, animation, collision, and camera systems

The project is still being built and may change often.

## Build and run

The current build setup is for macOS and uses Homebrew SDL2.

Install SDL2:

```sh
brew install sdl2
```

Generate the Makefiles, build the project, and run the editor:

```sh
./premake.sh gen
make config=debug
make run config=debug
```

## Project layout

- `src/` contains the application and engine startup code
- `core/`, `graphics/`, `input/`, and `managers/` contain the main engine parts
- `ecs/`, `components/`, and `systems/` contain game objects and game behavior
- `assets/` contains sprites, maps, shaders, audio, and source art
- `eclipse/` contains the engine library and third-party code

## Credits

- [Pikuma's SDL2 game programming course](https://pikuma.com/courses/2d-game-engine-cpp)
- [Hipo Engine](https://github.com/progrematic/hippo)
- [Godot](https://godotengine.org/)
- [LÖVE](https://love2d.org/)
- [raylib](https://www.raylib.com/)
- [MonoGame](https://monogame.net/)
