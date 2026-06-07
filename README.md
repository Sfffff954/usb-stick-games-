# NetPanel Games

Eine Sammlung von Arcade-Spielen in einer einzigen Windows-EXE. Gebaut mit reinem Win32 API und GDI, kein Framework.

## Spiele

### Snake
Klassisches Snake-Spiel. Frisst Futter, wird laenger, stirbt beim Waende oder sich selbst treffen.
Steuerung: WASD oder Pfeiltasten | Leertaste = Starten

### Pong
Tischtennis gegen KI. Erster mit 7 Punkten gewinnt.
Steuerung: Maus oder W/S | Leertaste = Starten

### Breakout
Alle Bloecke mit dem Ball zerstoeren. Mehrere Level, Bloecke brauchen mehrere Treffer.
Steuerung: Maus oder A/D | Leertaste = Starten

### Tetris
Klassisches Tetris mit Ghost-Piece und steigendem Tempo.
Steuerung: Pfeiltasten bewegen, Hoch drehen, Leertaste sofort fallen lassen

### DOOM
3D Raycaster-Shooter mit 4 Leveln und einem Endboss. Texturierte Waende, Boden und Decke. Maus-Aim, Auto-Aim, Minimap die sich aufdeckt waehrend man laeuft.
- 3 Waffen: Pistole, Shotgun, Plasma
- Feinde schiessen zurueck
- Boss in Level 4 mit 800 HP und 3-Wege-Schuss
- Steuerung: WASD bewegen, Maus zielen, LMB oder Leertaste schiessen, 1/2/3 Waffe wechseln, Q/E Strafe, TAB Karte, M Musik, R Neustart, ESC Verlassen

### Racing (Ausweichen)
Auf einer Strasse entgegenkommenden Autos, LKWs und Motorradern ausweichen. Geschwindigkeit steigt automatisch. Combo-Punkte fuer knappe Ausweichangaben.
Steuerung: A/D oder Pfeiltasten | R = Neustart nach Crash

### Space Shooter
Vertikaler Shoot-em-up. Wellen von Feinden in 3 Typen, Powerups, begrenzte Bomben.
- Powerups: Herz (HP), Blitz (Feuerkraft), Stern (Bombe)
- Steuerung: WASD bewegen, Z oder Leertaste schiessen, X Bombe, R Neustart

### Asteroids
Raumschiff mit Traegheitsphysik. Asteroiden splitten in kleinere Stuecke. Karte ist endlos (Wraparound).
Steuerung: A/D drehen, W Schub, Z oder Leertaste schiessen, R Neustart

### Pac-Man
Pac-Man Klon mit originalem Level-Layout. 4 Geister mit KI, Power-Pellets machen Geister fressbar.
Steuerung: WASD oder Pfeiltasten

### Weltkarte
199 echte Laender aus OpenStreetMap-Daten in Mercator-Projektion. Asteroiden und Atombomben auf Laender abfeuern. Laender haben HP und werden nach mehreren Treffern zerstoert.
- Linksklick: Land auswaehlen
- Rechtsklick: Zielposition setzen (roter Fadenkreuz)
- 1: Asteroid (unbegrenzt)
- 2: Atombombe (5 Stueck)
- ENTER: Abfeuern
- Mausrad: Zoom
- WASD: Karte verschieben

### Ski Downhill
First-Person Bergabfahrt. Baeumen und Steinen ausweichen, Muenzen einsammeln, Tore durchfahren. Geschwindigkeit steigt bis 280 km/h.
- Hindernisse: Baeume, Steine
- Sammeln: Goldmuenzen (Combo-Bonus), Tore (+50 Punkte + Tempo), Rampen (Sprung)
- 3 Leben
- Steuerung: A/D oder Pfeiltasten, R Neustart

## Steuerung (allgemein)

- Klick ins Spiel-Fenster zum Starten
- ESC: Zurueck zum Startbildschirm
- R: Neustart nach Game Over
- Mausrad: Zoom (Weltkarte, Krieg)

## Technisches

- Sprache: C (C99)
- API: Win32, GDI
- Kein Framework, keine externen Bibliotheken
- Kartendaten: Natural Earth / OpenStreetMap (vereinfacht)
- Kompiliert mit MinGW

## Bauen

```
gcc games.c -o NetPanelGames.exe -lgdi32 -lcomctl32 -lwinmm -mwindows -O2 -lm -s
```
