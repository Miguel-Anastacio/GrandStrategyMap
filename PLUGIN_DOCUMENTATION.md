# InteractiveGrandStrategyMap Plugin - User Documentation

## Table of Contents
TODO: Review at the end
1. [Overview](#overview)
2. [Features](#features)
3. [Plugin Installation](#plugin-installation)
4. [Map Generation Workflow](#map-generation-workflow)
5. [Integrating Map into Project](#integrating-map-into-project)
6. [Setting Up UI](#setting-up-ui)
7. [Plugin Hooks & Delegates](#plugin-hooks--delegates)
8. [Troubleshooting](#troubleshooting)

---

## Overview

The **InteractiveGrandStrategyMap** plugin provides a framework for creating interactive maps similar to those in grand strategy games like Europa Universalis 4 and Crusader Kings. It enables procedural map generation using Voronoi diagrams, interactive tile systems, dynamic textures, and data-driven gameplay.

### Supported Versions
- **Unreal Engine:** 5.4+, 
- **Platform:** Windows 64-bit

---

## Features

- **Procedural Map Generation** - Voronoi-based tile layouts from heightmaps
- **Interactive Tiles** - Clickable provinces with custom data structures
- **Dynamic Textures** - Real-time texture updates via compute shaders
- **Map Modes** - Switch between visual representations (Political, Terrain, etc.)
- **Camera Controllers** - Pre-configured BirdEye and Globe controllers
- **Blueprint Integration** - Full Blueprint support for integration into projects

---

## Plugin Installation

### Step 1: Enable the Plugin

1. Open your Unreal Engine project
2. Go to **Edit → Plugins**
3. Search for **"InteractiveGrandStrategyMap"**
4. Check **Enabled** checkbox
5. The plugin will be loaded

### Step 2: Configure Project Settings

The plugin requires the following project settings to be enabled:

#### Enable Support UV from Hit Results
1. Go to **Edit → Project Settings**
2. Search for **"Support UV from Hit Results"**
3. Check the **Enabled** checkbox
4. This is required for tile selection via raycasting

#### Enable Virtual Texturing (Optional but Recommended)
1. In **Project Settings**, search for **"Virtual Texture"**
2. Enable **Enable Virtual Texture Support**
3. This is used for large-scale map terrain textures
4. Recommended if using the example maps or large textures

### Step 3: Restart the Editor

Restart the editor to apply all changes and load the plugin fully.

---

## Map Generation Workflow

This section covers the process of generating a map in the editor before integrating it into your game. This can be optional, if you already have the necessary files for the map to work. 
These are: 
- Lookup table, a json containing ids and unique color for each tile
- Data table (json) holding the data for each tile in map form (id and data)
- Lookup texture
For information on how to set this check section X

### Prerequisites

Before you can generate a map, you need to prepare the following assets:

#### 1. Heightmap Texture (Required)
A 2D grayscale texture that defines the land/water distribution:
- **CompressionSettings**: UserInterface2D
- **SRGB:** Disabled (unchecked in texture settings)
- **Recommended Resolution:** 512×512 to 2048×2048
- **Size:** Must be power of 2 (512, 1024, 2048, 4096)
- **Content:** Grayscale values where brightness determines land/ocean (configurable via Cutoff Height)

#### 2. Border Texture (Optional)
If you want to upload custom borders for land or ocean tiles:
- **Same format requirements as heightmap**
- **Purpose:** The algorithm will generate tiles for one type (land or ocean) based on your heightmap, then use the border texture to define the borders
- **Note:** You can only upload borders for either land OR ocean, not both
- Use this to create specific coastline patterns or boundary shapes

### Step 1: Create the Custom Tile Data Structure

Define what data each tile will contain:

1. In Content Browser: **Right-click → Structure**
2. Create a new Blueprint Structure (e.g., `S_ProvinceData`)
3. Add propety ID (as an integer)
4. Add any other properties your tiles will have

Define another struct if you wish for ocean tiles.
These structures can also be defined in c++

### Step 2: Create a Map Object Asset

The Map Object stores the generated map data:

1. In Content Browser: **Right-click → MapEditor**
2. Select **Map Object** (class `UMapObject`) from the dropdown
3. Name it (e.g., `DA_MyMapData`)
4. Open the asset

### Step 3: Configure Map Generation Parameters

All map generation is controlled through parameters on the ClickableMap actor:

#### General Settings

| Parameter | Type | Default | Purpose |
|-----------|------|---------|---------|
| **Origin Texture** | Texture2D | None | **REQUIRED** - Your heightmap texture |
| **Cutoff Height** | Float (0.0-1.0) | 0.5 | Brightness threshold for land/ocean separation |
| **Land Below Cutoff** | Boolean | False | If true, darker areas = land; if false, brighter areas = land |
| **Upload Border** | Boolean | False | Whether to use a custom border texture |
| **Border Texture** | Texture2D | None | Custom border texture (if Upload Border is enabled) |

#### Land Details

Configure how land tiles are generated:

| Parameter | Type | Range | Purpose |
|-----------|------|-------|---------|
| **Number Of Tiles** | Int32 | 1-100000 | Target number of land tiles to generate |
| **Seed** | Int32 | Any | Random seed (change for different layouts with same heightmap) |
| **Lloyd Iteration** | Int32 | 0-20 | Relaxation iterations - higher = more regular, uniform tiles |
| **Search Radius** | Int32 | 1-1000 | Area to search before adding a new tile |

#### Ocean Details

Same structure as Land Details, controls water tile generation separately. You can have different numbers of land and ocean tiles.

#### Noise Details

Controls the roughness and appearance of tile borders:

| Parameter | Type | Range | Purpose |
|-----------|------|-------|---------|
| **Seed** | Int32 | Any | Noise randomization seed |
| **Octaves** | Int32 | 0-16 | Noise complexity (higher = more detailed borders) |
| **Frequency** | Float | 0.0-1.0 | Noise scale/zoom level |
| **Scale** | Float | 0.5-3.0 | Noise amplitude (how extreme the variations are) |
| **Line Thickness** | Float | 0.1-7.0 | Visual thickness of tile borders |

### Step 5: Generate the Map
1. Decide if you want to upload borders or not
2. Set height map texture
3. Set cutOff height, this determines which pixels are considered land or ocean
4. Set data structure for ocean and land tiles, if you do not need this separation a map with cutOff height = 0 will be only land tiles
5. Generate map (might take a bit), depends on number of tiles. 

There is no guarantee that the number of tiles will match exactly with the ones provided, it is more of a target.

### Step 6: Customize Tile Data (Optional)

After generation, you can edit individual tile data:

1. Open your **Map Object** asset
2. The asset editor shows all tiles and their data
3. Select individual tiles (through list or by clicking on tiles) and modify their properties, you can also select multiple and batch edit them

### Step 7: Set Up Visual Properties

Visual Properties define how tile data is represented visually:

1. Create a **Visual Property** definition (class `UVisualProperty`)
2. Map data properties to visual outputs:
   - **Example:** Temperature value (float) → Color gradient
   - **Example:** TerrainType (enum) → Specific colors per terrain
   - **Example:** Development (float) → Border thickness or pattern

3. Add these Visual Properties to your Map Object
4. The system uses these to render tile colors and effects

**IMPORTANT**: **You must add at least one visual property**

Some common types are provided like a string to color map.

#### Material Requirements

When creating custom materials for visual properties, ensure they include the required parameters:

**Required Parameters:**
- **LookupTexture** - Reference texture used for tile data lookup
- **Threshold** - Value threshold for visual representation

**Optional Features:**
- **Tile Highlighting** - To highlight hovered tiles, use the provided `MF_Highlight` material function. This enables visual feedback when the player hovers over tiles.
- **Tile Borders** - Custom border visualization between tiles can be implemented using provided border functions. Refer to example materials for implementation patterns.

**Dynamic Map Modes:**
For materials intended for dynamic map modes (e.g., political maps that update at runtime), ensure they include:
- **DynamicTexture** - Parameter for runtime texture updates via compute shaders

Refer to the example materials provided in the plugin for proper usage patterns.

#### Material Quality & Performance Note

The example materials provided with this plugin are basic implementations offered as starting points. They prioritize functionality over visual polish or performance optimization. You are encouraged to create custom materials tailored to your project's visual requirements and performance targets. Modify these materials as needed while maintaining the required parameters mentioned above.

---
## Loading your own data files and lookup texture

TODO: Add section here


---

## Integrating Map into Project


### Choosing Your Map Type

The plugin supports two types of interactive maps:

#### 1. Flat Map (BirdEyeController)
- **View:** Top-down, orthographic view
- **Use Case:** Traditional strategy game style (Europa Universalis, Civilization)
- **Best For:** 2D map representations
- **Controller:** BirdEyeController

#### 2. Globe Map (GlobeMapController)
- **View:** 3D spherical/orbital view
- **Use Case:** Planetary maps, global perspectives
- **Best For:** Immersive 3D experiences
- **Controller:** GlobeMapController

### Step 1: Controller, Pawn, Hud and GameMode

It is provided the following BPs:

**HUD:** `BP_Core_HUD`

#### Flat Map
- **Controller:** `BP_BirdEyeController`
- **GameMode** `BP_Core_FlatMapGameMode`
- **Pawn** `BP_Core_MapPawn`

#### Globe Map
- **Controller:** `BP_GlobeMapController`
- **GameMode** `BP_Core_GlobeMapGameMode`
- **Pawn** `BP_Core_GlobePawn`

Since this is provided as an engine plugin it is recommended to inherit from these, you can use the ones provided to get started.
For the controllers, the relevant inputs can be found implemented via input actions in the folder Inputs.

### Step 1: Create a Runtime ClickableMap Blueprint

Create the blueprint that will be used in your actual game levels:

1. **Right-click → Blueprint Class**
2. Search for and select **AClickableMap** as parent
3. Name it based on your map type (e.g., `BP_FlatInteractiveMap` or `BP_GlobeMap`)
4. Open the blueprint
5. In the Details panel, under **Map Parameters**:
   - **Set Map Object:** Select your generated `DA_MyMapData` asset
   - This links your blueprint to the pre-generated map data
6. Set your map mesh in MapNewGameplay (TODO: change this name)
7. Set the start map mode name (IMPORTANT)

### Step 2: Configure Level Settings

Set up your level to use the correct controller and pawn:

1. Open your game level
3. Under **Game Mode**, select one of the defined game modes or create a Game Mode Blueprint
4. In the Game Mode settings:
   - **Player Controller Class:** Set to your desired controller
     - Use `BirdEyeController` for flat maps
     - Use `GlobeMapController` for globe maps
   - **Default Pawn Class:** Set to the pawn compatible with your controller

### Step 3: Place the Map Actor

1. In your level, place an instance of your runtime ClickableMap blueprint
2. Ensure the **Map Object** property is set to your generated map asset
3. The map will be ready to use at runtime

For an example of a level where this is already setup please check the provided levels.

## Setting Up UI

The plugin includes the **PropertyDisplayGenerator** plugin to automatically generate UI for your data structures.
For detailed information on PropertyDisplayGenerator, check [PropertyDisplayGenerator Doc](https://docs.google.com/document/d/1Atnt15vsg1AsZ2PlTOnDwjJgdw_dszwZl24JnJ3NNWo/edit?usp=sharing)

### Asset Actions for UI Generation

To quickly generate all UI widgets required for your interactive map, use the automated asset action:

1. Select your `DA_MyMapData` in the Content Browser
2. Right-click and look for **Asset Actions**
3. Choose **Create Map UI Elements → Create All**

This will automatically create the following widgets using the PropertyDisplayGenerator plugin:

- **Data Display Widgets**: Two widgets (one for land tiles, one for ocean tiles) that display the properties of your custom data structures
- **Tile Selected Widget**: A container widget that combines the land and ocean data displays for showing selected tile information
- **Map Mode Selector Widget**: A widget for switching between different map visualization modes (based on your visual properties)
- **HUD Widget**: A main HUD widget that integrates the map mode selector and tile selected widget

These widgets provide a complete starting point for your map's UI. You can customize them further by modifying the generated blueprints. For more information on customizing the data display widgets, refer to the PropertyDisplayGenerator documentation.

Then open the HUD widget created via the action and set it's layout.

To have the new UI widgets, just set field `Hud Widget Class` in your HUD class with the HUD widget created via the action.

This UI setup is a quick starting point, if you wish you can completely ignore this and set up your own UI system. Since this is a very general solution, for your project you probably want something more customizable.

## Plugin Hooks & Delegates

Further integration and relevant events that can be useful to bind to.

### AClickableMap Delegates

The map broadcasts these events that you can listen to:

#### OnMapInitialized
Fires when map initialization completes and is ready for use.
```
Signature: void(AClickableMap* Map)
Usage: Bind to this to know when map data is available
```

#### OnMapTileChanged
Fires when one or more tiles are modified.
```
Signature: void(const TArray<int>& TileIDs)
Usage: Bind to update UI or gameplay when provinces change
Parameters: Array of tile IDs that were modified
```

#### OnMapModeChanged
Fires when the map mode switches.
```
Signature: void(FName OldMode, FName NewMode)
Usage: Bind to know when visualization changes
Parameters: Old and new mode names
```

TODO: Add events of controller
