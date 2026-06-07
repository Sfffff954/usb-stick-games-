# NetPanel Games

A collection of arcade games in a single Windows EXE. Built with pure Win32 API and GDI, no frameworks.

## Games

### Snake
Classic snake game. Eat food, grow longer, die on walls or yourself.
Controls: WASD or arrow keys | Space = start

### Pong
Table tennis against AI. First to 7 points wins.
Controls: Mouse or W/S | Space = start

### Breakout
Destroy all blocks with the ball. Multiple levels, blocks require several hits.
Controls: Mouse or A/D | Space = start

### Tetris
Classic Tetris with ghost piece and increasing speed.
Controls: Arrow keys to move, Up to rotate, Space to hard drop

### DOOM
3D raycaster shooter with 4 levels and a final boss. Textured walls, floor and ceiling. Mouse aim, auto-aim, minimap that reveals as you explore.
- 3 weapons: Pistol, Shotgun, Plasma
- Enemies shoot back
- Boss in level 4 with 800 HP and 3-way shot
- Controls: WASD move, Mouse aim, LMB or Space shoot, 1/2/3 switch weapon, Q/E strafe, TAB map, M music, R restart, ESC exit

### Racing (Dodge)
Dodge oncoming cars, trucks and motorcycles on a highway. Speed increases automatically. Combo points for near misses.
Controls: A/D or arrow keys | R = restart after crash

### Space Shooter
Vertical shoot-em-up. Waves of enemies in 3 types, powerups, limited bombs.
- Powerups: Heart (HP), Lightning (firepower), Star (bomb)
- Controls: WASD move, Z or Space shoot, X bomb, R restart

### Asteroids
Spaceship with inertia physics. Asteroids split into smaller pieces. Map wraps around.
Controls: A/D rotate, W thrust, Z or Space shoot, R restart

### Pac-Man
Pac-Man clone with original level layout. 4 ghosts with AI, power pellets make ghosts edible.
Controls: WASD or arrow keys

### World Map
199 real countries from OpenStreetMap data in Mercator projection. Fire asteroids and nukes at countries. Countries have HP and get destroyed after several hits.
- Left click: select country
- Right click: set target position (red crosshair)
- 1: Asteroid (unlimited)
- 2: Nuke (5 total)
- Enter: fire
- Mouse wheel: zoom
- WASD: pan map

### Ski Downhill
First-person downhill skiing. Dodge trees and rocks, collect coins, go through gates. Speed increases up to 280 km/h.
- Obstacles: trees, rocks
- Collect: gold coins (combo bonus), gates (+50 points + speed), ramps (jump)
- 3 lives
- Controls: A/D or arrow keys, R restart

## General Controls

- Click game window to start
- ESC: back to start screen
- R: restart after game over
- Mouse wheel: zoom (World Map)

## Technical

- Language: C (C99)
- API: Win32, GDI
- No framework, no external libraries
- Map data: Natural Earth / OpenStreetMap (simplified)
- Compiled with MinGW

## Build

```
gcc games.c -o NetPanelGames.exe -lgdi32 -lcomctl32 -lwinmm -mwindows -O2 -lm -s
```
