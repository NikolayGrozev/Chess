# Chess

A terminal-based offline chess game written in C++ using object-oriented design. Supports mouse input for piece selection, move highlighting, game history navigation, save/replay, and standard chess rules including castling, en passant, and the 50-move draw rule.

---

## Features

- Interactive console board with mouse-driven piece selection
- Legal move highlighting on piece selection
- Full move history — step forward and backward through any game without affecting live play
- Save a game to file and replay it move by move
- Standard draw conditions: 50-move rule and insufficient material
- Tracks material balance for both sides throughout the game

---

## Architecture

The project is structured around five core classes:

| Class | Responsibility |
|---|---|
| `chessPiece` | Abstract base for all pieces — holds color, pin state, and move generation interface |
| `ChessBoard` | Owns the 8×8 grid, executes and validates moves, tracks material |
| `Movement` | Encodes a single move as a from/to pair — used for history, undo, save, and replay |
| `Renderer` | Reads board state and draws it to the console |
| `Inputhandler` | Captures mouse clicks and translates console coordinates to board positions |
| `game` | Orchestrates all subsystems — owns the board, move history, turn state, and the main loop |

The piece hierarchy applies all four pillars of OOP:

- **Encapsulation** — piece color, pin flag, and move state are private, accessed through typed getters and setters
- **Abstraction** — `get_ValidMoves()` is pure virtual; callers never need to know which piece they hold
- **Inheritance** — `pawn`, `knight`, `bishop`, `rook`, `queen`, and `king` all extend `chessPiece`
- **Polymorphism** — the board calls `get_ValidMoves()` through a `chessPiece*`; dispatch is resolved at runtime

Move history is stored as a flat array of `Movement` objects. Rather than saving board snapshots, the game replays from move 0 to move N to reconstruct any past position. This keeps memory usage proportional to the number of moves played, not the number of positions visited.

---

## Build

Requires `g++` and `make` on Windows.

```bash
make
```

This compiles `classes.cpp` to an object file and links it with `main.cpp` to produce `main.exe`.

```bash
main.exe
```

To clean build artifacts:

```bash
make clean
```

---

## Project Structure

```
chess/
├── main.cpp          # Entry point
├── classes.cpp       # All class definitions
├── makefile          # Build rules
└── README.md
```

---