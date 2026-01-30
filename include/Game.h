#pragma once

#include "AiPlayer.h"

#include <SDL.h>
#include <future>
#include <string>
#include <vector>

#include "Board.h"
#include "Button.h"
#include "Move.h"
#include "stb_truetype.h"

class Game
{
public:
    Game();
    ~Game();
    void aiTurn();
    void play();
    bool tick();
    void drawLoop();

private:
    bool playFrame();
    void drawUiFrame();
    void draw();
    void drawYellowSquare(float x, float y);
    void drawRedSquare(float x, float y);
    void drawFilledCircle(int centerX, int centerY, int radius, const SDL_Color &color) const;
    void drawHighlightSquare(int x, int y, int size) const;
    SDL_Point measureText(const std::string &text) const;
    void drawText(const std::string &text, int x, int y, const SDL_Color &color) const;
    void checkGameOver();
    void getMoves();
    bool canMove(Move &m) const;
    void reset();
    void basicWindowM(std::future<void> &f);
    void drawUi();
    void enforceSquareWindow(int newWidth, int newHeight);
    SDL_Point eventToLogical(int x, int y) const;
    SDL_Point windowToLogical(int x, int y) const;
    void logClickDebug(const char* phase, int windowX, int windowY, const SDL_Point &logical, int viewSquareX, int viewSquareY) const;
    bool isBoardFlipped() const;
    SDL_Point viewToBoardSquare(int viewX, int viewY) const;
    SDL_Point boardToViewSquare(int boardX, int boardY) const;

    static constexpr int logicalSize = 800;

    unsigned int windowSize;
    SDL_Window* window;
    SDL_Renderer* renderer;
    Board board;
    const Piece* heldPiece;
    const Piece* recentPiece;
    bool inCheck;
    std::vector<Move> whiteMoves;
    std::vector<Move> blackMoves;
    std::vector<Move> playedMoves;
    bool gameOver;
    AiPlayer ai;
    bool aiStarted;
    bool aiIsWhite;
    bool aiPending;
    bool menuActive;
    SDL_Cursor* click;
    SDL_Cursor* arrow;
    bool debugClicks;
    SDL_Point lastMouseLogical;
    bool fontLoaded;
    float fontScale;
    int fontAscent;
    int fontDescent;
    int fontLineGap;
    std::vector<unsigned char> fontBuffer;
    stbtt_fontinfo fontInfo;

    SDL_Color lightSquareColor;
    SDL_Color darkSquareColor;
    SDL_Color yellowHighlightColor;
    SDL_Color redHighlightColor;
    SDL_Color moveHintColor;
    SDL_Color outlineColor;
    SDL_Color highlightFillColor;
};
