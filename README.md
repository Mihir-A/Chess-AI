<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/Mihir-A/Chess-AI">
    <img src="images/logo.png" alt="Logo" width="60" height="60">
  </a>

<h3 align="center">Chess Ai</h3>

  <p align="center">
    A chess game implemented using C++ and SFML, with an optional AI
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

## Features
* Support for castling and auto promotion to queen
* AI by default looks 4 moves into the future
* AI uses minimax with alpha-beta prunning
* Ai uses piece value and piece position to rank moves


<!-- GETTING STARTED -->
## Building from scratch
  
### Prerequisites

* C++ compiler with c++17 support (VS 20+ or gcc recomened)
  
### Installation

1. Download [SFML](https://www.sfml-dev.org/download/sfml/2.5.1/)
2. Clone the repo
   ```sh
   git clone https://github.com/Mihir-A/Chess-AI.git
   cd Chess-AI
   ```
3. Add SFML libs to lib folder
4. Build using Make, CMake, or the Visual Studio Solution
   ```sh
   make all
   ```
   Or with CMake:
   ```sh
   cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
   cmake --build build
   ```
5. Run either chess.exe or chess file depending on OS making sure the assets folder is in the same directory

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
