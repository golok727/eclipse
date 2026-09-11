# Engine Study Order

This is the exact order to study the project. Open one file at a time and do
not jump ahead until the current file makes sense.

The engine is written in C++. You do not need to understand every C++ feature
immediately. First learn what data each file owns and who calls each function.

## Stage 0: Start Here

### 1. `src/main.cpp`

This is the program entry point.

- `CreateApp()` creates the sample editor/game.
- `Engine::Run()` starts the engine.

Next file: `src/main.h`.

### 2. `src/main.h`

This declares the small launcher contract used by `main.cpp`.

Next file: `src/app.h`.

### 3. `src/app.h`

This is the base application interface. A game provides these functions:

- `GetWindowProperties()` chooses the window settings.
- `Initialize()` creates the first scene.
- `Update()` runs game-specific frame logic.
- `Shutdown()` releases game resources.
- `ImGuiRender()` draws editor tools.

Next file: `src/engine.h`.

## Stage 1: The Engine Loop

### 4. `src/engine.h`

This lists the engine services and systems that live for the whole program.
Read the member variables before reading the implementation.

Next file: `src/engine.cpp`.

### 5. `src/engine.cpp`

Read these functions in this order:

1. `Run()` starts the main loop.
2. `Initialize()` creates the window and services.
3. `Update()` runs one frame of game logic.
4. `Render()` draws one frame.
5. `Shutdown()` closes everything in reverse order.

The most important idea is the frame loop: input, systems, app logic, camera,
then rendering.

Next file: `core/clock.h`.

### 6. `core/clock.h`

The clock gives the frame time called `deltaTime`. Movement uses this value so
the player moves at the same speed on fast and slow computers.

Next file: `core/clock.cpp`.

### 7. `core/clock.cpp`

Read `Tick()` to see how the time between frames is measured.

Next file: `core/window.h`.

## Stage 2: Window and Input

### 8. `core/window.h`

This describes the game window and the off-screen framebuffer used by the
editor's GameView panel.

Next file: `core/window.cpp`.

### 9. `core/window.cpp`

Read these functions:

- `Create()` opens SDL and creates OpenGL.
- `BeginRender()` prepares the framebuffer.
- `EndRender()` displays the result and draws ImGui.
- `PollEvents()` receives keyboard and window events.

Next file: `core/imguiwindow.h`.

### 10. `core/imguiwindow.h`

This is the small wrapper around ImGui. It keeps editor UI setup separate from
the game window.

Next file: `core/imguiwindow.cpp`.

### 11. `core/imguiwindow.cpp`

Read `Create()`, `BeginRender()`, and `EndRender()` to understand when ImGui
starts and finishes a frame.

Next file: `input/keyboard.h`.

### 12. `input/keyboard.h`

This contains the engine's keyboard names, such as `ECLIPSE_INPUT_KEY_A`.

Next file: `input/keyboard.cpp`.

Read `Update()`, `Key()`, and `KeyDown()`.

Next file: `input/actions.h`.

### 13. `input/actions.h`

This converts raw keys into game actions such as `MoveLeft` and `Attack`.
Game systems should ask for actions instead of knowing keyboard numbers.

Next file: `input/mouse.h`.

### 14. `input/mouse.h`

This declares mouse position and button queries.

Next file: `input/mouse.cpp`.

Read how mouse state is updated and queried.

## Stage 3: Textures and OpenGL Drawing

You already know the texture part. Continue from there in this order.

### 15. `graphics/texture.h`

The texture object owns an image and its OpenGL texture ID.

### 16. `graphics/texture.cpp`

Read the constructor, `LoadTexture()`, `Bind()`, and `SetTextureFilter()`.

Next file: `graphics/shader.h`.

### 17. `graphics/shader.h`

The shader is the small program that runs on the graphics card. Uniform
functions send values such as color, camera matrices, and texture coordinates.

Next file: `graphics/shader.cpp`.

Read shader creation first, then `Bind()`, then the uniform functions.

Next file: `graphics/mesh.h`.

### 18. `graphics/mesh.h`

A mesh is the shape drawn on screen. The sample uses one square.

Next file: `graphics/mesh.cpp`.

Read how vertex positions, texture coordinates, and triangle indices are sent
to OpenGL.

Next file: `graphics/rendercommands.h`.

### 19. `graphics/rendercommands.h`

A render command is a saved instruction to draw something later.

Next file: `graphics/rendercommands.cpp`.

Read `RenderMeshTextured::Execute()`. This is where the mesh, texture, shader,
and camera matrices meet.

Next file: `managers/rendermanager.h`.

### 20. `managers/rendermanager.h`

This stores drawing commands until the engine is ready to execute them.

Next file: `managers/rendermanager.cpp`.

Read `Submit()`, `Flush()`, `Clear()`, and framebuffer handling.

Next file: `systems/render_system.h`.

### 21. `systems/render_system.h`

This declares the system that turns entities into render commands.

Next file: `systems/render_system.cpp`.

Read `Render()` carefully. It collects sprites, sorts them by layer, and sends
them to the render manager.

Next files: `assets/shaders/sprite.vert` and `assets/shaders/sprite.frag`.

These are the two small programs used by the sprite renderer. The vertex shader
positions the square. The fragment shader chooses the pixel color.

## Stage 4: Entities and Components

### 22. `ecs/entity.h`

An entity is just a number used to identify a game object.

Next file: `ecs/world.h`.

### 23. `ecs/world.h`

The world stores entities and their components. Study these functions:

1. `CreateEntity()` creates an ID.
2. `Add()` attaches data to an entity.
3. `Get()` finds data on one entity.
4. `ForEach()` lets a system process matching entities.
5. `DestroyEntity()` marks an entity for removal.
6. `FlushDestroyed()` actually removes it safely.

Next file: `components/rendercomponents.h`.

### 24. `components/rendercomponents.h`

Read these data types:

- `Transform`: position, rotation, and size.
- `SpriteRenderer`: mesh, shader, image, frame coordinates, and layer.
- `Camera`: visible area and zoom.

Next file: `components/gameplaycomponents.h`.

### 25. `components/gameplaycomponents.h`

Read these data types:

- `PlayerController`: player movement speed.
- `Collider`: rectangle used by collision code.
- `Animation`: current frame and sprite-sheet layout.
- `NpcBehavior`: patrol points and movement speed.

The important rule is that components store data. Systems contain the actions.

## Stage 5: Gameplay Systems

### 26. `systems/movement_system.h`

This declares player movement.

Next file: `systems/movement_system.cpp`.

Read how actions become a direction and how `deltaTime` changes position.

Next file: `systems/animation_system.h`.

### 27. `systems/animation_system.h`

This declares the system that advances animation frames.

Next file: `systems/animation_system.cpp`.

Read how elapsed time selects a column and row in a sprite sheet.

Next file: `systems/collision_system.h`.

### 28. `systems/collision_system.h`

This declares the basic rectangle collision system.

Next file: `systems/collision_system.cpp`.

Read `Overlaps()` first, then the penetration correction in `Update()`.

Next file: `systems/npc_system.h`.

### 29. `systems/npc_system.h`

This declares NPC behavior updates.

Next file: `systems/npc_system.cpp`.

Read how an NPC moves between two patrol points.

Next file: `systems/camera_system.h`.

### 30. `systems/camera_system.h`

This declares camera updates.

Next file: `systems/camera_system.cpp`.

Read how world coordinates become a visible rectangle and how aspect ratio is
preserved.

## Stage 6: Managers and Scenes

### 31. `managers/assetmanager.h`

This describes the cache for textures and shaders.

Next file: `managers/assetmanager.cpp`.

Read how a path is used as a cache key and how files are loaded once.

Next file: `managers/layeredtilemaploader.h`.

### 32. `managers/layeredtilemaploader.h`

This declares the loader for the exported background PNG layers.

Next file: `managers/layeredtilemaploader.cpp`.

Read how each image becomes a background entity with a different render layer.

Next file: `managers/tiledmaploader.h`.

### 33. `managers/tiledmaploader.h`

This declares the older Tiled `.tmx` loader.

Next file: `managers/tiledmaploader.cpp`.

Read this later. It is useful when you move from exported images to a real
tilemap with tile IDs and collision layers.

Next file: `managers/scenemanager.h`.

### 34. `managers/scenemanager.h`

This describes named scenes and the function that builds each scene.

Next file: `managers/scenemanager.cpp`.

Read how loading a scene clears the old world and calls its builder.

Next file: `managers/gamestatemanager.h`.

### 35. `managers/gamestatemanager.h`

This stores whether the game is playing, paused, in a menu, or over.

## Stage 7: The Sample Game

### 36. `editor/editor_app.h`

This lists the data owned by the sample editor/game.

Next file: `editor/editor_app.cpp`.

Read `Initialize()` in sections:

1. Create the square mesh.
2. Load textures and shaders.
3. Create the player entity.
4. Load the map layers.
5. Create NPCs and walls.
6. Create the camera.

Then read `Update()` for player animation selection.

Finally read `ImGuiRender()` for the tools and GameView panel.

### 37. `systems/pathfinding.h`

This describes a grid and the search result frames.

Next file: `systems/pathfinding.cpp`.

Read `FindPathTrace()` to see A* compare possible cells until it reaches the
goal. The editor's A* window visualizes these frames.

## Stage 8: Build Files and Project Shape

### 38. `premake5.lua`

This tells Premake which files belong to the engine library and editor app.
Read the `eclipse` project first, then `eclipseeditor`.

### 39. `premake.sh`

This is the small helper that runs Premake and preserves the custom Makefile
include.

### 40. `README.md`

Read this last for the normal build commands and project overview.

## What To Build Next

## How To Write An Actual Game

The engine is the reusable machinery. Your game is the rules and content built
on top of it. Do not put every game idea into `src/engine.cpp`.

### Keep Engine Code And Game Code Separate

Engine code belongs in these folders:

- `ecs/`: entity storage.
- `components/`: reusable data types.
- `systems/`: reusable rules that process components.
- `graphics/`: OpenGL drawing.
- `input/`: keyboard and mouse input.
- `managers/`: assets, scenes, audio, and game states.

Your game code should eventually live in folders such as:

```text
game/
  game_app.h
  game_app.cpp
  components/
  systems/
  scenes/
  data/
assets/
  game/
```

The current `editor/EditorApp` is a sample game and editor combined. Use it to
learn, then gradually move your own player, enemies, scenes, and rules into a
`game/` folder.

### Build A Small Complete Game First

Start with one simple game loop:

1. The player starts in a scene.
2. The player can move.
3. An enemy patrols or follows the player.
4. The player can attack or reach a goal.
5. The game detects success or failure.
6. A message appears for win or lose.
7. A restart action loads the scene again.

Do not begin with a giant RPG. Finish one small game loop first. A finished
small game teaches more than a large unfinished engine.

### Add A Player

Start in `editor/editor_app.cpp` and find the player creation code.

Give the player these components:

- `Transform` for position and size.
- `SpriteRenderer` for the image.
- `PlayerController` for movement speed.
- `Collider` for contact with walls or enemies.
- `Animation` for the sprite sheet.

Then follow the player through these systems:

1. `input/actions.h` reads the requested action.
2. `systems/movement_system.cpp` changes the position.
3. `systems/collision_system.cpp` corrects illegal overlap.
4. `systems/animation_system.cpp` changes the displayed frame.
5. `systems/render_system.cpp` draws the player.

### Add An Enemy

Create another entity with a `Transform`, `SpriteRenderer`, `Collider`, and
`Animation`. Add a new `EnemyBehavior` component in the game folder, then add a
game system that changes the enemy position.

For a simple enemy, start with this order:

1. Patrol between two points.
2. Detect the player inside a distance.
3. Ask `AStarPathfinder` for a route.
4. Move through the route one grid cell at a time.
5. Damage the player when close enough.

Study `systems/npc_system.cpp` before writing the enemy system. It is the
smallest behavior example in the project.

### Add A Goal And Game Rules

Create a goal entity with a collider or a custom `Goal` component. In a game
system, check whether the player overlaps it. When that happens:

```cpp
Engine::Instance().GetGameState().Set(managers::GameState::GameOver);
```

For a real game, add a separate `Won` state later. The current Game State
window is only a debugging tool; replace it with your own game HUD and menus.

### Make Separate Scenes

Do not keep every level inside one `Initialize()` function. Create scene builder
functions such as:

```text
game/scenes/main_menu_scene.cpp
game/scenes/forest_scene.cpp
game/scenes/battle_scene.cpp
```

Register each builder with `SceneManager`, then load them by name. A scene
builder should create entities and configure the starting camera. It should
not contain the engine loop or OpenGL code.

### Save Game Content

The current sample still creates entities in C++ and does not yet save scenes
from the editor. Until scene serialization is added, use one builder function
per scene and commit those files with the game. The next editor feature to add
is saving and loading transforms, sprites, colliders, and animation settings
to a scene file.

### A Practical First Project

Build a small top-down game with this plan:

1. Main menu scene.
2. One map scene using the tilemap layers.
3. One controllable character.
4. Three patrol enemies.
5. One purple A* route used by an enemy.
6. Three collectible objects.
7. A win state after collecting all objects.
8. A lose state after the player is hit three times.
9. Pause and restart buttons.
10. A final build that starts directly in the menu.

This uses the systems already in the engine and gives you a real game to
extend with better art, lighting, sound, menus, and story.

The engine is good enough for small 2D games now. It is still a learning
engine, so expect to improve collision, save files, map editing, audio, and
asset packaging as your game requires them.
