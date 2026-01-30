<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/Mihir-A/Chess-AI">
    <img src="images/logo.png" alt="Logo" width="60" height="60">
  </a>

<h3 align="center">Chess Ai</h3>

  <p align="center">
    A chess game implemented using C++ and SDL2, with an optional AI
  </p>
</div>

<div align="center">
  <img src="images/screenshot.png" alt="Screenshot" width="512" height="540">
</div>


<!-- USAGE EXAMPLES -->
## Usage
* Select if you want to play an AI or a 2 player game
* Moves can be done by dragging a piece or clicking on a piece and then clicking the new square
* Press R to resset game or D to undo a move
* Toggle AI with A
* Play online at https://mihirdev.com/chess

## Features
* Support for castling and auto promotion to queen
* AI by default looks 4 moves into the future
* AI uses minimax with alpha-beta prunning
* Ai uses piece value and piece position to rank moves


<!-- GETTING STARTED -->
## Building from scratch
  
### Prerequisites

* C++ compiler with C++23 support (VS 2022+ or modern gcc/clang recommended)
  
### Installation

1. Clone the repo
   ```sh
   git clone https://github.com/Mihir-A/Chess-AI.git
   cd Chess-AI
   ```
   SDL2 is fetched automatically by CMake (no separate install needed).
3. Build with CMake (recommended):
   ```sh
   cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
   cmake --build build
   ```
   The binary will be at `build/release/chess` on macOS/Linux or `build\\release\\chess.exe` on Windows.
4. Run the binary, making sure the `assets` folder is in the same directory.

### WebAssembly (Emscripten)
Use Emscripten to produce a browser build. Outputs go to `build/wasm`.

1. Configure with `emcmake`:
   ```sh
   emcmake cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
   ```
2. Build:
   ```sh
   cmake --build build
   ```
3. Open `build/wasm/chess.html` in a local web server.

If you previously configured `build/` with a native toolchain, clear it before switching:
```sh
rm -rf build/CMakeCache.txt build/CMakeFiles
```

## Running
Make sure the `assets` folder sits next to the executable.

### macOS (Gatekeeper warning)
If macOS blocks the app (“not opened”), you can:
1. Right‑click the `chess` binary → Open → Open.
2. Or System Settings → Privacy & Security → “chess was blocked” → Open Anyway.
3. Or clear quarantine in Terminal:
   ```sh
   xattr -d com.apple.quarantine /path/to/chess
   ```

### Windows
Run `chess.exe` from the build output folder. If SmartScreen warns, choose “More info” → “Run anyway”.

### Linux
Run the `chess` binary from the build output folder. If needed, `chmod +x chess` first.

## Screenshots
<p float="left">
  <img src="images/intro.png" alt="Screenshot" width="256" height="270">
  <img src="images/ss2.png" alt="Screenshot" width="256" height="270">
</p>


<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE.md` for more information.



<!-- CONTACT -->
## Contact

mihiranan@gmail.com

Project Link: [https://github.com/Mihir-A/Chess-AI](https://github.com/Mihir-A/Chess-AI)



<!-- ACKNOWLEDGMENTS -->
## Acknowledgments
* [Chess.com](Chess.com) Used for Neo pieces
* [Chess-AI](https://github.com/SebLague/Chess-AI/blob/d0832f8f1d32ddfb95525d1f1e5b772a367f272e/Assets/Scripts/Core/PieceSquareTable.cs#L4) Used for Piece square tables
* [Readme Template](https://github.com/othneildrew/Best-README-Template)

<p align="right">(<a href="#readme-top">back to top</a>)</p>
