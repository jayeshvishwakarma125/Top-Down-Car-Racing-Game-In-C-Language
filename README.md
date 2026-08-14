# Car Racing Game

A simple **2D Car Racing Game** developed using **C/C++ and Turbo C++ Graphics (BGI)**. 
The game allows the player to control a car, avoid enemy vehicles, collect fuel cans, and achieve the highest possible score.

## Features

* Player-controlled racing car
* Multiple enemy cars
* Fuel consumption system
* Fuel cans that can be collected to refill fuel
* Collision detection between cars
* Roadside scenery with trees and rocks
* Background music and sound effects
* High-score system
* Pause functionality
* Three difficulty levels:

  * Easy
  * Medium
  * Hard
* Double buffering for smoother graphics
* Restart/replay option after game over

## Controls

| Key       | Action                         |
| --------- | ------------------------------ |
| `A` / `←` | Move car left                  |
| `D` / `→` | Move car right                 |
| `P`       | Pause / Resume                 |
| `X`       | Exit                           |
| `M`       | Return to menu after Game Over |

## Fuel System

The player starts with a maximum fuel level of **100**.

* Fuel continuously decreases while the game is running.
* Collecting a fuel can restores **20 fuel points**.
* The maximum fuel level is capped at **100**.
* The game ends if the fuel reaches `0`.

Collecting a fuel can also gives the player **50 bonus points**.

## Scoring System

* **+10 points** → Successfully passing an enemy car
* **+50 points** → Collecting a fuel can
* High scores are automatically stored in `HIGHSCORE.TXT`.

## 🎚️ Difficulty Levels

| Difficulty | Enemy Speed | Game Delay |
| ---------- | ----------- | ---------- |
| Easy       | 8           | 50 ms      |
| Medium     | 11          | 40 ms      |
| Hard       | 14          | 30 ms      |

Higher difficulty means faster enemy cars and faster gameplay.

## 🛠️ Technologies Used

* **C/C++**
* **Turbo C++**
* **BGI Graphics Library**
* `graphics.h`
* `conio.h`
* `dos.h`
* `stdio.h`
* `stdlib.h`
* `time.h`

## 📂 Project Structure

```text
Car-Racing-Game/
│
├── car_racing.cpp
├── HIGHSCORE.TXT
└── README.md
```

> `HIGHSCORE.TXT` is automatically created/updated by the game to store the highest score.

## ⚙️ How to Run

This project uses the **Turbo C++ BGI graphics environment**, so it is intended to run in a Turbo C++/DOSBox environment.

### 1. Install Turbo C++

Set up Turbo C++ with the BGI graphics library.

### 2. Place the source code

Place the `.cpp` source file inside your Turbo C++ project directory.

### 3. Configure BGI

The program initializes graphics using:

```cpp
initgraph(&gd, &gm, "C:\\TURBOC3\\BGI");
```

Make sure the BGI folder exists at the specified location, or update the path according to your setup.

### 4. Compile and Run

Compile the program in Turbo C++ and run it.

## Game Logic

The game continuously performs the following operations:

```text
Start Game
    ↓
Select Difficulty
    ↓
Initialize Player, Enemies & Scenery
    ↓
Draw Game Screen
    ↓
Read Keyboard Input
    ↓
Update Game Logic
    ↓
Move Enemy Cars
    ↓
Consume Fuel
    ↓
Check Collisions
    ↓
Update Score
    ↓
Repeat
    ↓
Game Over
    ↓
Save High Score
    ↓
Restart / Exit
```

## Highlights

### Collision Detection

The game checks collisions between:

* Player car ↔ Enemy cars
* Player car ↔ Fuel cans
* Fuel cans ↔ Enemy cars

### High Score

The highest score is loaded when the game starts and saved automatically when a new high score is achieved.

### Double Buffering

The game uses two graphics pages to reduce screen flickering and provide smoother animation.

## Game Screens

You can add screenshots of:

* Main Menu
* Gameplay
* Fuel System
* Pause Screen
* Game Over Screen

Example:

```markdown
![Main Menu](screenshots/menu.png)
![Gameplay](screenshots/gameplay.png)
![Game Over](screenshots/gameover.png)
```

## Future Improvements

Some possible improvements include:

* Add more types of vehicles
* Add different road environments
* Add levels and missions
* Add power-ups
* Add a leaderboard
* Add more sound effects
* Add improved graphics and animations
* Port the game to a modern graphics framework

## Author

**Jiya Bhandari**

A C/C++ graphics-based mini project demonstrating game development concepts such as:

* Keyboard input handling
* Graphics programming
* Collision detection
* File handling
* Game loops
* Score management
* Randomization
* Sound effects
* Double buffering
