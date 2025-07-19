# How to create a Game with Electrine

Create your 2D (automatically) side scrolling platformer with Electrine Engine!
Electrine provides you with:

- Entity Component System consisting of:
    - \ref gl3::engine::ecs::EntityFactory as wrapper to quickly create EnTT entities with some default Components (Tag,
      Transform, Render, Physics, Group, Parent).
    - \ref gl3::engine::ecs::EventDispatcher to quickly dispatch game or ui events.
    - \ref gl3::engine::ecs::System that you can inherit from to create your own systems.
      As well as some preset events to use for the game and ui.
- \ref gl3::engine::rendering::RenderingSystem and gl3::engine::rendering::TextureManager to render colored or textured
  triangles and quads.
- \ref gl3::engine::physics::PhysicsSystem and gl3::engine::physics::PlayerContactListener to move and handle
  contacts/collision via physics bodies.
  > **Tip:** Physics already set each entities transform to their physics transform after the step (also moves grouped
  entities (have GroupComponent) relative to their parent. The contact listener already checks if the player hits an
  object from the left, or hits an entity tagged "
  obstacle", and sends a PlayerDeath event you can subscribe to.
- \ref gl3::engine::audio::AudioSystem and \ref gl3::engine::audio::AudioAnalysis for soundtrack and SFX playback and
  BPM as well as onset analysis.
- \ref gl3::engine::ui::UISystem to which you can register your own custom (preferably minimal) ImGui UIs (as \ref gl3::
  engine::ui::IUISubsystem), that it will automatically update. (Includes a \ref gl3::engine::ui::FontManager for
  loading fonts to ImGui)
- \ref gl3::engine::state::StateManagementSystem, that you can use in your own state machine to update a stack of \ref
  gl3::engine::state::GameState, that you implement, like a pause, level, menu state, etc.
- \ref gl3::engine::editor::EditorSystem and \ref gl3::engine::editor::EditorUISystem to add to your game and connect to
  their events for quick level generation.
    - As well as \ref gl3::engine::levelLoading::LevelManager and \ref gl3::engine::levelLoading::LevelSelectUISystem
      for level saving and loading, including de-/serialization functionality for json.

> **Note:** The \ref gl3::engine::context::Context handles window functionality, and for a minimal game, you only need
> to inherit from \ref gl3::engine::Game and run it!

## Creating the Game

Please follow the [QuickStart](../QuickStart.md) guide first, to get the repository and correct setup!
Then:

- Inherit your Game from the engine's Game.h and initialize it in your main file.
- Then setup everything in your Game class as you need! E.g. generate entities with the EntityFactory and don't forget
  to save your player entity in your Game class for the engine systems to use (e.g. for automatic collision checking)!

```cpp
//Create the main game instance with window size, title, camera position, and zoom level. (Zooming after initialization not implemented in context, leave it at 1/100)
       
        Game YourGame(
            1280, // Window width
            720, // Window height
            "ElectronXPulse", // Window title
            glm::vec3(0.0f, 0.0f, 1.0f), // Initial camera position
            1.0 / 100.f // Camera zoom standard value
        );

// Run the main game loop. (Could call start() before this, but don't need to)
        YourGame.run();
```