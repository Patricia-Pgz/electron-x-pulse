# How to play ElectronXPulse

Leap through vibrant 2D worlds perfectly synced to the rhythm of high-energy EDM tracks! With dynamic visuals and
music-driven gameplay, each level challenges your timing and reflexes.  
ElectronXPulse is an auto-scrolling 2D platformer where you jump to the beat and master each song’s rhythm to reach the
end. All you need is one simple input: press **Space** to jump — and the fun can begin! Each level features a unique
soundtrack, design and difficulty for you to conquer.

## How to use the in-game Level Editor

Here are some tips for using the level editor!

- First of all, clone and set up ElectronXPulse as described
  in [QuickStart](../QuickStartGame.md).  
  Once you run the game, you start off in the level selection screen. The Edit Mode button is in the top left corner of
  this screen. Click it and select the level that you want to edit.

\image html ../images/EditButton.png
\image latex ../images/EditButton.png

- If you want to **add** a new level, you must add it as
  json file to the assets/levels folder before starting the application, as well as a .meta.json for previewing it in
  the
  level selection. Look at the pre-made levels in this folder, to get an impression of the necessary data to preview the
  level.  
  Textures for the preview go into assets/uiTextures, textures for the actual game go to assets/textures (except for "
  background" and "ground" textures, that will be fitted to the screen divided by groundLevel).
  > **Note:** Every texture from the texture folders gets loaded into the game on application start up, so make sure to
  only put in the textures, that you really use.

  The **meta file** needs the following information:

  **`Tutorial.meta.json`**

  ```json5
      {
    "id": 0,
    // a unique identifier
    "name": "Tutorial",
    // the preview name
    "levelFile": "Tutorial.json",
    // the file name to the corresponding level
    "previewImg": "tutorial"
    // the name of the image to show as preview in level select (lies in assets/uiTextures)
  }
  ```

  The **level file** must contain at least the following:

  **`Tutorial.json`**

  ```json5
  
  {
    //obligatory
    "audioFile": "SensesShorter.wav",
    // the filename of the audio track in assets/audio
    "velocityMultiplier": 1,
    // a multiplier for the automatically on BPM computed level speed 
    "playerStartPosX": -2,
    // the player's initial x-position, needed for some calculations
  
    //optional:
    "groundLevel": -1.5,
    // the y-coordinate at which ground and sky part
    "clearColor": [],
    // solid background color
    // top and  bottom color for a gradient sky entity
    "gradientTopColor": [],
    "gradientBottomColor": [],
    // array of sky/ground objects, sky tagged "background", ground tagged "ground"
    "backgrounds": [],
    // objects grouped under 1 physics parent
    "groups": [],
    // single game objects
    "objects": []
  }
  ```

- In the editor you just need to click on a grid cell and place a Texture or select a solid color from the Tile Panel on
  the right. If you want something mor specific, follow the steps in the tile panel!
    - In the top part of the tile panel, there is a button to save the level! Don't forget to use it. When you selected
      cell(s), a button appears for deletion. Here, you also find a toggle to switch from "Single Select" to "
      MultiSelect"

\image html ../images/Editor.png
\image latex ../images/Editor.png
\image html ../images/Color.png
\image latex ../images/Color.png

- In **Multi Select** mode, you also have the option to group objects under one physics parent (e.g. to keep the player
  from
  stuttering over two adjacent objects, that each have their own collider). Activate **"Generate Group Physics Collider
  before selecting cells to group"**. Once you place a tile on the selection it gets highlighted in blue, and you can
  finalize the group with the **"Generate Group"** button.

\image html ../images/Group.png
\image latex ../images/Group.png

Example for an element in "objects", "backgrounds" (or children in "groups"):
> **Note:** You can create the objects in the editor

```json5
    {
  "position": [
    -2,
    -1,
    0
  ],
  "scale": [
    0.8,
    0.8,
    1
  ],
  "rotation": 0,
  "color": [
    1,
    0,
    0,
    1
  ],
  "tag": "player",
  // Don't forget to tag the player
  "isTriangle": false,
  "textureName": "geometry",
  "uv": [
    // standard uv for single texture (not tile)
    0,
    0,
    1,
    1
  ],
  "generatePhysicsComp": true,
  "generateRenderComp": true,
  // automatically uses default shaders
  "vertexShaderPath": "",
  "fragmentShaderPath": "",
  // doesn't use the following, is only default:
  "gradientTopColor": [
    1,
    1,
    1,
    1
  ],
  "gradientBottomColor": [
    1,
    1,
    1,
    1
  ],
  "parallaxFactor": 0
  // for background parallax effect, gets multiplied by level speed
}
```