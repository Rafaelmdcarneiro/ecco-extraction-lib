# EccoLib

A graphics extraction library for the Ecco the Dolphin series.

<img src="https://raw.githubusercontent.com/JohnnyLdeAlba/ecco-lib/main/eccolib-stage.png" alt="" style="width: 40%;" /> <img src="https://raw.githubusercontent.com/JohnnyLdeAlba/ecco-lib/main/vortex-hybrid.png" alt="" style="width: 40%;" /> 


# Requirements

The below libraries are only necessary for displaying and outputting images to PNG files.

- Direct2D (d2d1.lib)
- Windows Imaging Component (windowscodecs.lib)

# Supported Games

- Ecco the Dolphin - Palettes, Tiles, Blocks, Sprites, Stage Sprites and Level Maps
- Ecco the Dolphin PC - Palettes, Tiles, Blocks, Sprites and Level Maps
- Ecco 2: The Tides of Time - Palettes, Blocks, Sprites, Stage Sprites and Level Maps
- Ecco Jr - Palettes, Tiles, Blocks, Sprites and Level Maps

# What's the Purpose of the data Directory?

The `data` directory contains save states from Ecco the Dolphin and Ecco 2.
Currently EccoLib does not support decompressors needed for tiles and other data.
To remedy this issue, the `data` folder is provided with save states with all
the data already compressed. Support for decompressors will be added soon.

# Todo

- Add tile and block decompressors for Ecco the Dolphin.
- Add tile and level data decompressors for Ecco 2: The Tides of Time.
- Add stage sprite extraction support for Ecco PC.

# Special Thanks

I would like to thank Leia Ivon Flame of RedDashGames whose research contributed toward the creation of the extraction tool that generated the Ecco PC Level Maps.

# Galleries with Images Extracted Using this Library
- [Ecco 2: The Tides of Time Sprites](https://eccothedolphin.online/ecco-2-the-tides-of-time/sprites)
- [Ecco 2: The Tides of Time Level Maps](https://eccothedolphin.online/ecco-2-the-tides-of-time/level-maps)
