# C++ SFML Chess Engine

A fully functional, object-oriented Chess Engine built from scratch in C++ using the **Simple and Fast Multimedia Library (SFML)** for rendering. 

This project implements the complete set of standard chess rules—including complex mechanics like En Passant and Castling—while offering a robust, interactive Graphical User Interface (GUI) complete with a main menu, dynamic move validation, and a full game replay system.

---

## 🌟 Key Features

* **Complete Rule Compliance:** Accurately implements all chess rules, including precise move validation, Check/Checkmate detection, Stalemate, the 50-move rule, Insufficient Material draws, En Passant, and Castling.
* **Interactive SFML GUI:** A responsive 1000x1000 graphical interface featuring visual move highlighting, turn indicators, and interactive clickable buttons.
* **Menu-Driven Architecture:** A seamless main menu system allowing users to jump into active gameplay or browse past replays without restarting the engine.
* **Auto-Saving & Replay System:** Matches are automatically logged chronologically into a `saves/` directory. The built-in Replay Menu dynamically scans and lists these files, allowing users to step backward and forward through past games move-by-move.

---

## 🏗️ Class Architecture

The engine is built on strong Object-Oriented Programming (OOP) principles to strictly separate logical game states from graphical rendering.

### `chessPiece` (Base Class)
An abstract base class representing a generic chess piece. It tracks the `pieceColor` (White/Black), `material` value, and `hasMoved` state. 
* **Inherited Classes:** `pawn`, `rook`, `knight`, `bishop`, `queen`, `king`.
* Each derived class uniquely implements the abstract `legalMoves` function to generate mathematically valid pseudo-legal moves for its specific type.

### `ChessBoard`
The core state manager. It maintains the `MutableBoardMatrix`—an 8x8 2D array of `chessPiece*` pointers.
* Responsibilities include placing pieces, executing moves, capturing pieces, managing En Passant targets, and validating whether a specific board state places a King in Check.

### `Renderer`
Encapsulates all SFML drawing logic to ensure the game logic remains agnostic of the graphics library.
* Maps internal 8x8 grid coordinates to actual on-screen pixel coordinates.
* Loads dynamic textures from the `assets/` directory and renders the board, piece sprites, valid-move highlighting circles, and menu UI overlays.

### `game`
The supreme orchestrator. It manages the active gameplay loop and user inputs.
* Tracks the `currentTurn`, move history vector, and validates game-ending scenarios (Checkmate, Stalemate, Surrender).
* Handles the `game_replay` loop, tearing down the board and seamlessly rebuilding historical board states based on the loaded save file.

---

## 📦 Dependencies

To compile and run this project, you will need:

1. **C++17 Compiler:** (e.g., `g++` via MinGW-w64 on Windows). The code utilizes modern features like `<filesystem>`.
2. **SFML 2.5+:** The Simple and Fast Multimedia Library. 
   * **Modules Used:** `sfml-graphics`, `sfml-window`, `sfml-system`.
3. **Assets Folder:** Ensure the `assets/` directory (containing PNG sprites and the `ArialCE.ttf` font) remains in the project root.
4. **Saves Folder:** A `saves/` directory is required in the root for the engine to write and read match logs.

---

## 🚀 How to Start (Compilation & Execution)

This repository includes a `makefile` pre-configured to link the necessary SFML libraries.

### 1. Clone the Repository
Pull the repository to your local machine and navigate into the root directory.

### 2. Ensure SFML is Configured
Ensure that your C++ compiler can find the SFML include and library paths. If you are on Windows using MinGW, ensure the SFML `bin` folder (containing the `.dll` files) is added to your system's PATH environment variable, or place the required `.dll` files directly next to the executable.

### 3. Compile the Project
Open your terminal in the project root and run the make command:
```bash
mingw32-make main.exe
```
*(Note: Use `make main.exe` if you are on Linux or using a standard Make installation).*

### 4. Launch the Engine
Run the newly compiled executable to launch the Main Menu:
```bash
./main.exe
```

### 5. Cleanup (Optional)
To clean up compiled object files `.o` and the executable, you can run:
```bash
mingw32-make clean
```