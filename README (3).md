# 🐍 Snake Game (C++)

A console-based **Snake Game** built in C++ demonstrating core programming concepts — loops, arrays, conditional logic, dynamic board display, win/loss detection, and replay functionality.

---

## 🎮 Features

- ✅ Real-time snake movement on a 20×40 grid
- ✅ WASD and Arrow key controls
- ✅ Dynamic board redrawn every tick
- ✅ Food spawning at random positions
- ✅ Score tracking (+10 per food eaten)
- ✅ High score across rounds
- ✅ Wall collision and self-collision detection
- ✅ Replay option after game over
- ✅ Colour-coded display (green snake, red food)

---

## 🧠 C++ Concepts Demonstrated

| Concept | Where Used |
|---|---|
| 2D Array | Game grid `char grid[20][40]` |
| Loops | Game loop, board rendering, collision check |
| Conditional logic | Direction control, collision detection |
| `deque` (STL) | Snake body — push front (head), pop back (tail) |
| `class` | `SnakeGame` encapsulates all game logic |
| `struct` | `Point` for coordinates, `Direction` enum |

---

## 🗂️ Project Structure

```
snake-game/
├── snake.cpp     # Full C++ source code
├── Makefile      # Build configuration
├── .gitignore    # Git exclusions
└── README.md     # Project documentation
```

---

## ⚙️ How to Compile & Run

### Using Make (recommended)
```bash
make
./snake
```

### Using g++ directly
```bash
g++ -std=c++17 -Wall -Wextra -o snake snake.cpp
./snake
```

### On Windows (MinGW)
```bash
g++ -std=c++17 -o snake.exe snake.cpp
snake.exe
```

---

## 🕹️ Controls

| Key | Action |
|---|---|
| `W` / `↑` | Move Up |
| `S` / `↓` | Move Down |
| `A` / `←` | Move Left |
| `D` / `→` | Move Right |
| `Q` | Quit game |

---

## 📌 Requirements

- C++17 or later
- GCC / G++ (Linux, macOS, or Windows via MinGW)
- Terminal with ANSI colour support

---

## 👨‍💻 Author

Developed as part of a C++ Mini Game Project assignment.
