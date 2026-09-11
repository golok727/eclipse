# Learning After Textures

The engine is now a good small-game foundation, but it is not a finished
commercial engine. Learn it in this order:

## 1. Shader

Open `graphics/shader.h` and `graphics/shader.cpp`.

- A shader is a tiny program that runs on the graphics card.
- `Bind()` makes a shader the active drawing program.
- Uniforms are values sent from C++ to the shader.
- `SetUniformMat4()` sends position and camera information.

Then read `assets/shaders/sprite.vert` and `sprite.frag`.

## 2. Mesh

Open `graphics/mesh.h` and `graphics/mesh.cpp`.

- A mesh is the shape being drawn.
- The engine uses one square with four corners.
- Texture coordinates tell the shader which part of an image to show.

Look at `EditorApp::Initialize()` to see the square being created.

## 3. Render Commands

Open `graphics/rendercommands.h` and `graphics/rendercommands.cpp`.

- A render command is a request to draw something later.
- The render system prepares commands.
- The render manager executes them in order.

Follow `RenderSystem::Render()` next.

## 4. ECS

Open `ecs/world.h`.

- An entity is just an ID.
- A component is data, such as position or animation state.
- A system is code that processes entities with particular components.

Read `CreateEntity()`, `Add()`, `Get()`, and `ForEach()`.
Then compare `MovementSystem`, `AnimationSystem`, and `RenderSystem`.

## 5. Components

Open `components/rendercomponents.h` and `components/gameplaycomponents.h`.

- `Transform` says where an entity is and how large it is.
- `SpriteRenderer` says which image and shader it uses.
- `Animation` says how a sprite sheet is divided into frames.
- `Collider` describes a solid shape.

To add a new game feature, start by deciding what data it needs, then add a
component for that data.

## 6. Systems

Open `src/engine.cpp` and study `Engine::Update()`.

The engine runs systems every frame. Follow this order:

1. Input is collected by `input/keyboard.cpp`.
2. Movement changes `Transform` data.
3. NPC behavior changes NPC transforms.
4. Collision corrects positions.
5. Animation changes sprite frame coordinates.
6. The app chooses game-specific behavior.
7. The camera prepares the view.

Systems should focus on one job. That makes them easier to replace later.

## 7. Scenes

Open `managers/scenemanager.h` and `editor/editor_app.cpp`.

- A scene is a collection of entities that represents one place or mode.
- `EditorApp::Initialize()` builds the current scene.
- `SceneManager::Load()` clears the old world and builds a named scene.

The next useful improvement is splitting the sample scene into separate files
such as `MainMenuScene`, `TownScene`, and `BattleScene`.

## 8. Input Actions

Open `input/actions.h`.

Do not make every game system know about raw keyboard codes. Instead, systems
ask for actions such as `MoveLeft` or `Attack`. Later you can let players
rebind those actions without changing movement code.

## 9. Camera and Viewport

Open `systems/camera_system.cpp`.

- The camera decides which part of the world is visible.
- The viewport is the actual window size.
- `fitSize` and `fitScale` preserve the map proportions.

When the game grows, add camera limits, camera shake, and smooth following.

## 10. Pathfinding

Open `systems/pathfinding.cpp` and the `A* Pathfinding Demo` window.

- A grid stores walkable and blocked cells.
- A* checks possible cells and prefers the cheapest promising route.
- `FindPath()` returns the final route.
- `FindPathTrace()` exposes the search so the editor can show it.

The next step is giving an NPC a target entity and moving it through the
returned path one cell at a time.

## 11. What Makes It A Game

Build one small game before adding many engine features. Add one scene, one
player goal, a few NPCs, a win condition, a lose condition, and a restart.
Only add an engine system when the game needs it.
