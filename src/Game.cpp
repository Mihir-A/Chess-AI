#define STB_TRUETYPE_IMPLEMENTATION
#include "Game.h"

#include "King.h"
#include "Piece.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <future>
#include <iostream>
#include <iterator>
#include <string>
#include <thread>

namespace {

const char* pieceTypeToString(Piece::Type type)
{
    switch (type) {
    case Piece::Type::Pawn:
        return "Pawn";
    case Piece::Type::Rook:
        return "Rook";
    case Piece::Type::Queen:
        return "Queen";
    case Piece::Type::King:
        return "King";
    case Piece::Type::Bishop:
        return "Bishop";
    case Piece::Type::Knight:
        return "Knight";
    }
    return "Unknown";
}

std::string describePiece(const Piece* piece)
{
    if (piece == nullptr) {
        return "none";
    }
    const char* color = piece->isWhite() ? "white" : "black";
    return std::string(color) + " " + pieceTypeToString(piece->getPieceType());
}

} // namespace

Game::Game()
    : windowSize(800)
      , window(nullptr)
      , renderer(nullptr)
      , heldPiece(nullptr)
      , recentPiece(nullptr)
      , inCheck(false)
      , gameOver(false)
      , ai(board)
      , aiStarted(false)
      , aiIsWhite(false)
      , aiPending(false)
      , menuActive(true)
      , click(nullptr)
      , arrow(nullptr)
      , debugClicks(false)
      , lastMouseLogical{0, 0}
      , fontLoaded(false)
      , fontScale(1.0f)
      , fontAscent(0)
      , fontDescent(0)
      , fontLineGap(0)
      , fontBuffer()
      , fontInfo{}
      , lightSquareColor{240, 217, 181, 255}
      , darkSquareColor{181, 136, 99, 255}
      , yellowHighlightColor{255, 255, 0, 130}
      , redHighlightColor{235, 97, 80, 204}
      , moveHintColor{0, 0, 0, 25}
      , outlineColor{255, 255, 255, 166}
      , highlightFillColor{255, 255, 255, 32}
{
    if (const char* env = std::getenv("CHESS_DEBUG_CLICKS"); env != nullptr) {
        debugClicks = std::string(env) != "0";
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << '\n';
        return;
    }

    // Prefer crisper scaling when the logical render size is scaled.
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    window = SDL_CreateWindow(
        "Chess",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        static_cast<int>(windowSize),
        static_cast<int>(windowSize),
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (window == nullptr) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << '\n';
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << '\n';
        return;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderSetLogicalSize(renderer, logicalSize, logicalSize);

    click = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
    arrow = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    SDL_SetCursor(arrow);

    {
        std::ifstream fontFile("assets/Montserrat-Regular.ttf", std::ios::binary);
        if (!fontFile) {
            std::cerr << "Failed to open font file: assets/Montserrat-Regular.ttf\n";
        }
        else {
            fontBuffer.assign(std::istreambuf_iterator<char>(fontFile), std::istreambuf_iterator<char>());
            if (fontBuffer.empty()) {
                std::cerr << "Font file was empty: assets/Montserrat-Regular.ttf\n";
            }
            else {
                const int offset = stbtt_GetFontOffsetForIndex(fontBuffer.data(), 0);
                if (offset < 0 || stbtt_InitFont(&fontInfo, fontBuffer.data(), offset) == 0) {
                    std::cerr << "Failed to initialize stb_truetype font.\n";
                }
                else {
                    fontLoaded = true;
                    fontScale = stbtt_ScaleForPixelHeight(&fontInfo, 32.0f);
                    stbtt_GetFontVMetrics(&fontInfo, &fontAscent, &fontDescent, &fontLineGap);
                }
            }
        }
    }

    Piece::loadTextures(renderer);

    playedMoves.reserve(10);

    std::string fen;
    std::cout << "Enter FEN: ";
    //std::getline(std::cin, fen);
    if (fen.length() > 5) {
        board.decipherFen(fen);
    }
    getMoves();
}

Game::~Game()
{
    Piece::unloadTextures();

    if (click != nullptr) {
        SDL_FreeCursor(click);
        click = nullptr;
    }
    if (arrow != nullptr) {
        SDL_FreeCursor(arrow);
        arrow = nullptr;
    }

    if (renderer != nullptr) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window != nullptr) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    SDL_Quit();
}

SDL_Point Game::windowToLogical(int x, int y) const
{
    if (renderer == nullptr || window == nullptr) {
        return SDL_Point{x, y};
    }

    int logicalW = logicalSize;
    int logicalH = logicalSize;
    int w = 0;
    int h = 0;
    SDL_RenderGetLogicalSize(renderer, &w, &h);
    if (w > 0 && h > 0) {
        logicalW = w;
        logicalH = h;
    }

    int windowW = 0;
    int windowH = 0;
    SDL_GetWindowSize(window, &windowW, &windowH);
    if (windowW <= 0 || windowH <= 0) {
        return SDL_Point{-1, -1};
    }

    const float scaleX = static_cast<float>(logicalW) / static_cast<float>(windowW);
    const float scaleY = static_cast<float>(logicalH) / static_cast<float>(windowH);
    const float lx = static_cast<float>(x) * scaleX;
    const float ly = static_cast<float>(y) * scaleY;

    if (lx < 0.0f || ly < 0.0f || lx >= static_cast<float>(logicalW) || ly >= static_cast<float>(logicalH)) {
        return SDL_Point{-1, -1};
    }

    return SDL_Point{static_cast<int>(std::floor(lx)), static_cast<int>(std::floor(ly))};
}

SDL_Point Game::eventToLogical(int x, int y) const
{
    int logicalW = logicalSize;
    int logicalH = logicalSize;
    if (renderer != nullptr) {
        int w = 0;
        int h = 0;
        SDL_RenderGetLogicalSize(renderer, &w, &h);
        if (w > 0 && h > 0) {
            logicalW = w;
            logicalH = h;
        }
    }

    if (x < 0 || y < 0 || x >= logicalW || y >= logicalH) {
        return SDL_Point{-1, -1};
    }

    return SDL_Point{x, y};
}

bool Game::isBoardFlipped() const
{
    return aiIsWhite;
}

SDL_Point Game::viewToBoardSquare(int viewX, int viewY) const
{
    if (!isBoardFlipped()) {
        return SDL_Point{viewX, viewY};
    }
    return SDL_Point{7 - viewX, 7 - viewY};
}

SDL_Point Game::boardToViewSquare(int boardX, int boardY) const
{
    if (!isBoardFlipped()) {
        return SDL_Point{boardX, boardY};
    }
    return SDL_Point{7 - boardX, 7 - boardY};
}

void Game::logClickDebug(const char* phase, int windowX, int windowY, const SDL_Point &logical, int viewSquareX, int viewSquareY) const
{
    if (!debugClicks || renderer == nullptr || window == nullptr) {
        return;
    }

    int windowW = 0;
    int windowH = 0;
    SDL_GetWindowSize(window, &windowW, &windowH);

    int outputW = 0;
    int outputH = 0;
    SDL_GetRendererOutputSize(renderer, &outputW, &outputH);

    SDL_Rect viewport{};
    SDL_RenderGetViewport(renderer, &viewport);

    float scaleX = 0.0f;
    float scaleY = 0.0f;
    SDL_RenderGetScale(renderer, &scaleX, &scaleY);

    int logicalW = 0;
    int logicalH = 0;
    SDL_RenderGetLogicalSize(renderer, &logicalW, &logicalH);

    const float logicalWf = static_cast<float>(logicalSize);
    const float logicalHf = static_cast<float>(logicalSize);
    const float outScaleX = outputW > 0 ? static_cast<float>(outputW) / logicalWf : 1.0f;
    const float outScaleY = outputH > 0 ? static_cast<float>(outputH) / logicalHf : 1.0f;
    const float outScale = std::min(outScaleX, outScaleY);
    const float outViewportW = logicalWf * outScale;
    const float outViewportH = logicalHf * outScale;
    const float outViewportX = (static_cast<float>(outputW) - outViewportW) * 0.5f;
    const float outViewportY = (static_cast<float>(outputH) - outViewportH) * 0.5f;

    const bool viewValid = viewSquareX >= 0 && viewSquareX < 8 && viewSquareY >= 0 && viewSquareY < 8;
    const SDL_Point boardSquare = viewValid ? viewToBoardSquare(viewSquareX, viewSquareY) : SDL_Point{-1, -1};
    const bool boardValid = boardSquare.x >= 0 && boardSquare.x < 8 && boardSquare.y >= 0 && boardSquare.y < 8;
    const Piece* centerPiece = boardValid ? board.getPiece(boardSquare.x, boardSquare.y) : nullptr;
    const Piece* rightPiece = (boardValid && boardSquare.x + 1 < 8) ? board.getPiece(boardSquare.x + 1, boardSquare.y) : nullptr;
    const Piece* leftPiece = (boardValid && boardSquare.x - 1 >= 0) ? board.getPiece(boardSquare.x - 1, boardSquare.y) : nullptr;

    constexpr int tileSize = logicalSize / 8;
    const int tileOriginX = viewValid ? viewSquareX * tileSize : -1;
    const int tileOriginY = viewValid ? viewSquareY * tileSize : -1;

    std::cerr
        << "[click:" << phase << "] "
        << "win=(" << windowX << "," << windowY << ") "
        << "logical=(" << logical.x << "," << logical.y << ") "
        << "square=(" << viewSquareX << "," << viewSquareY << ") "
        << "board=(" << boardSquare.x << "," << boardSquare.y << ") "
        << "tileOrigin=(" << tileOriginX << "," << tileOriginY << ") "
        << "windowSize=(" << windowW << "x" << windowH << ") "
        << "outputSize=(" << outputW << "x" << outputH << ") "
        << "logicalSize=(" << logicalW << "x" << logicalH << ") "
        << "viewport=(" << viewport.x << "," << viewport.y << "," << viewport.w << "," << viewport.h << ") "
        << "scale=(" << scaleX << "," << scaleY << ") "
        << "computedOutputViewport=("
        << outViewportX << "," << outViewportY << "," << outViewportW << "," << outViewportH << ") "
        << "computedOutputScale=" << outScale << " "
        << "pieces{left=" << describePiece(leftPiece)
        << ", center=" << describePiece(centerPiece)
        << ", right=" << describePiece(rightPiece) << "}"
        << '\n';
}

void Game::enforceSquareWindow(int newWidth, int newHeight)
{
    const int newSize = std::max(1, std::min(newWidth, newHeight));
    if (static_cast<int>(windowSize) != newSize) {
        windowSize = static_cast<unsigned int>(newSize);
        SDL_SetWindowSize(window, newSize, newSize);
        // Re-apply logical sizing to ensure the viewport stays in sync.
        if (renderer != nullptr) {
            SDL_RenderSetLogicalSize(renderer, logicalSize, logicalSize);
        }
    }

    // Synchronize with the actual window size (e.g., on high-DPI displays).
    int actualW = 0;
    int actualH = 0;
    SDL_GetWindowSize(window, &actualW, &actualH);
    windowSize = static_cast<unsigned int>(std::max(1, std::min(actualW, actualH)));
}

void Game::aiTurn()
{
    Move aiM = ai.getBestMove();
    playedMoves.push_back(aiM);
    board.makeMove(aiM);
    board.changeTurn();
    getMoves();
}

void Game::play()
{
    menuActive = true;
    drawUi();

    bool running = window != nullptr && renderer != nullptr;
    while (running) {
        running = playFrame();
    }
}

bool Game::tick()
{
    if (window == nullptr || renderer == nullptr) {
        return false;
    }

    if (menuActive) {
        drawUiFrame();
        return window != nullptr && renderer != nullptr;
    }

    return playFrame();
}

bool Game::playFrame()
{
    if (window == nullptr || renderer == nullptr) {
        return false;
    }

    bool running = true;
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
        else if (event.type == SDL_MOUSEMOTION) {
            lastMouseLogical = eventToLogical(event.motion.x, event.motion.y);
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN) {
            const SDL_Point mouse = eventToLogical(event.button.x, event.button.y);
            lastMouseLogical = mouse;
            if (mouse.x < 0 || mouse.y < 0) {
                logClickDebug("down-outside", event.button.x, event.button.y, mouse, -1, -1);
                continue;
            }
            const int viewX = mouse.x / (logicalSize / 8);
            const int viewY = mouse.y / (logicalSize / 8);
            const SDL_Point boardSquare = viewToBoardSquare(viewX, viewY);
            const int xCord = boardSquare.x;
            const int yCord = boardSquare.y;
            logClickDebug("down", event.button.x, event.button.y, mouse, viewX, viewY);

            if (xCord < 8 && xCord > -1 && yCord < 8 && yCord > -1) {
                const Piece* clickedPiece = board.getPiece(xCord, yCord);
                if (clickedPiece != nullptr && clickedPiece->isWhite() == board.isWhiteTurn()) {
                    heldPiece = clickedPiece;
                }
                else if (recentPiece != nullptr) {
                    auto attemptedMove = Move(recentPiece, board.getPiece(xCord, yCord), recentPiece->getX(), recentPiece->getY(), xCord, yCord);

                    if (canMove(attemptedMove) && recentPiece->isWhite() == board.isWhiteTurn()) {
                        board.makeMove(attemptedMove);
                        playedMoves.push_back(attemptedMove);
                        board.changeTurn();
                        getMoves();
                        recentPiece = nullptr;
                    }
                }
            }
        }
        else if (event.type == SDL_MOUSEBUTTONUP && heldPiece != nullptr) {
            const SDL_Point mouse = eventToLogical(event.button.x, event.button.y);
            lastMouseLogical = mouse;
            if (mouse.x < 0 || mouse.y < 0) {
                logClickDebug("up-outside", event.button.x, event.button.y, mouse, -1, -1);
                heldPiece = nullptr;
                continue;
            }
            const int viewX = mouse.x / (logicalSize / 8);
            const int viewY = mouse.y / (logicalSize / 8);
            const SDL_Point boardSquare = viewToBoardSquare(viewX, viewY);
            const int xCord = boardSquare.x;
            const int yCord = boardSquare.y;
            logClickDebug("up", event.button.x, event.button.y, mouse, viewX, viewY);

            if (xCord < 8 && xCord > -1 && yCord < 8 && yCord > -1) {
                if (heldPiece == recentPiece) {
                    recentPiece = nullptr;
                }
                else {
                    recentPiece = heldPiece;
                }

                auto attemptedMove = Move(heldPiece, board.getPiece(xCord, yCord), heldPiece->getX(), heldPiece->getY(), xCord, yCord);

                if (canMove(attemptedMove)) {
                    board.makeMove(attemptedMove);
                    playedMoves.push_back(attemptedMove);
                    board.changeTurn();
                    getMoves();
                    recentPiece = nullptr;
                }
            }
            heldPiece = nullptr;
        }
        else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_d) {
            if (!playedMoves.empty()) {
                board.unmakeMove(playedMoves.back());
                playedMoves.pop_back();
                board.changeTurn();
                getMoves();
            }
            if (!playedMoves.empty()) {
                board.unmakeMove(playedMoves.back());
                playedMoves.pop_back();
                board.changeTurn();
                getMoves();
            }
            gameOver = false;
        }
        else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r) {
#ifdef __EMSCRIPTEN__
            if ((event.key.keysym.mod & (KMOD_CTRL | KMOD_GUI)) != 0) {
                continue;
            }
#endif
            reset();
        }
        else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
            enforceSquareWindow(event.window.data1, event.window.data2);
        }
        else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_a) {
            aiStarted = !aiStarted;
        }
    }

    draw();

#ifdef __EMSCRIPTEN__
    if (aiPending) {
        if (board.isWhiteTurn() == aiIsWhite && aiStarted && !gameOver) {
            aiTurn();
        }
        aiPending = false;
    }
    else if (board.isWhiteTurn() == aiIsWhite && aiStarted && !gameOver) {
        // Defer AI to the next frame so the browser can present the player's move first.
        aiPending = true;
    }
#else
    if (board.isWhiteTurn() == aiIsWhite && aiStarted && !gameOver) {
        auto f = std::async(std::launch::async, &Game::aiTurn, this);
        basicWindowM(f);
    }
#endif

    if (!running || window == nullptr || renderer == nullptr) {
        return false;
    }

    return true;
}

void Game::drawLoop()
{
    draw();
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            SDL_DestroyWindow(window);
            window = nullptr;
        }
    }
}

void Game::drawYellowSquare(float x, float y)
{
    SDL_SetRenderDrawColor(renderer, yellowHighlightColor.r, yellowHighlightColor.g, yellowHighlightColor.b, yellowHighlightColor.a);
    SDL_Rect rect{static_cast<int>(x), static_cast<int>(y), 100, 100};
    SDL_RenderFillRect(renderer, &rect);
}

void Game::drawRedSquare(float x, float y)
{
    SDL_SetRenderDrawColor(renderer, redHighlightColor.r, redHighlightColor.g, redHighlightColor.b, redHighlightColor.a);
    SDL_Rect rect{static_cast<int>(x), static_cast<int>(y), 100, 100};
    SDL_RenderFillRect(renderer, &rect);
}

void Game::drawFilledCircle(int centerX, int centerY, int radius, const SDL_Color &color) const
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (int dy = -radius; dy <= radius; ++dy) {
        const int dx = static_cast<int>(std::sqrt(radius * radius - dy * dy));
        SDL_RenderDrawLine(renderer, centerX - dx, centerY + dy, centerX + dx, centerY + dy);
    }
}

void Game::drawHighlightSquare(int x, int y, int size) const
{
    const int thickness = 5;

    SDL_Rect fill{x, y, size, size};
    SDL_SetRenderDrawColor(renderer, highlightFillColor.r, highlightFillColor.g, highlightFillColor.b, highlightFillColor.a);
    SDL_RenderFillRect(renderer, &fill);

    SDL_SetRenderDrawColor(renderer, outlineColor.r, outlineColor.g, outlineColor.b, outlineColor.a);
    for (int i = 0; i < thickness; ++i) {
        SDL_Rect r{x + i, y + i, size - i * 2, size - i * 2};
        if (r.w > 0 && r.h > 0) {
            SDL_RenderDrawRect(renderer, &r);
        }
    }
}

SDL_Point Game::measureText(const std::string &text) const
{
    SDL_Point size{0, 0};
    if (!fontLoaded || text.empty()) {
        return size;
    }

    int width = 0;
    int prev = 0;
    for (unsigned char ch : text) {
        const int codepoint = static_cast<int>(ch);
        if (prev != 0) {
            width += static_cast<int>(stbtt_GetCodepointKernAdvance(&fontInfo, prev, codepoint) * fontScale);
        }
        int advance = 0;
        int leftBearing = 0;
        stbtt_GetCodepointHMetrics(&fontInfo, codepoint, &advance, &leftBearing);
        width += static_cast<int>(advance * fontScale);
        prev = codepoint;
    }

    const float height = static_cast<float>(fontAscent - fontDescent + fontLineGap) * fontScale;
    size.x = width;
    size.y = static_cast<int>(height);
    return size;
}

void Game::drawText(const std::string &text, int x, int y, const SDL_Color &color) const
{
    if (!fontLoaded || renderer == nullptr || text.empty()) {
        return;
    }

    int penX = x;
    const int baseline = y + static_cast<int>(fontAscent * fontScale);
    int prev = 0;

    for (unsigned char ch : text) {
        const int codepoint = static_cast<int>(ch);

        if (prev != 0) {
            penX += static_cast<int>(stbtt_GetCodepointKernAdvance(&fontInfo, prev, codepoint) * fontScale);
        }

        int advance = 0;
        int leftBearing = 0;
        stbtt_GetCodepointHMetrics(&fontInfo, codepoint, &advance, &leftBearing);

        int w = 0;
        int h = 0;
        int xoff = 0;
        int yoff = 0;
        unsigned char* bitmap = stbtt_GetCodepointBitmap(&fontInfo, 0.0f, fontScale, codepoint, &w, &h, &xoff, &yoff);

        if (bitmap != nullptr && w > 0 && h > 0) {
            SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA32);
            if (surface != nullptr) {
                Uint32* pixels = static_cast<Uint32*>(surface->pixels);
                const int pixelCount = w * h;
                for (int i = 0; i < pixelCount; ++i) {
                    const Uint8 alpha = static_cast<Uint8>((static_cast<int>(bitmap[i]) * color.a) / 255);
                    pixels[i] = SDL_MapRGBA(surface->format, color.r, color.g, color.b, alpha);
                }

                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_FreeSurface(surface);

                if (texture != nullptr) {
                    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
                    SDL_Rect dst{penX + xoff, baseline + yoff, w, h};
                    SDL_RenderCopy(renderer, texture, nullptr, &dst);
                    SDL_DestroyTexture(texture);
                }
            }
            stbtt_FreeBitmap(bitmap, nullptr);
        }

        penX += static_cast<int>(advance * fontScale);
        prev = codepoint;
    }
}

void Game::draw()
{
    if (renderer == nullptr) {
        return;
    }

    SDL_SetRenderDrawColor(renderer, lightSquareColor.r, lightSquareColor.g, lightSquareColor.b, lightSquareColor.a);
    SDL_RenderClear(renderer);

    constexpr int boardSize = logicalSize;
    constexpr int gSize = boardSize / 8;

    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            const bool dark = (x + y) % 2 == 1;
            const SDL_Color &color = dark ? darkSquareColor : lightSquareColor;
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_Rect rect{x * gSize, y * gSize, gSize, gSize};
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    if (!playedMoves.empty()) {
        const Move &lastMove = playedMoves.back();
        const SDL_Point viewNew = boardToViewSquare(lastMove.getNewX(), lastMove.getNewY());
        const SDL_Point viewOld = boardToViewSquare(lastMove.getOldX(), lastMove.getOldY());
        drawYellowSquare(static_cast<float>(gSize * viewNew.x), static_cast<float>(gSize * viewNew.y));
        drawYellowSquare(static_cast<float>(gSize * viewOld.x), static_cast<float>(gSize * viewOld.y));
    }

    if (inCheck) {
        const auto &pieces = board.isWhiteTurn() ? board.getWhitePieces() : board.getBlackPieces();
        for (const auto piece : pieces) {
            if (piece->getPieceType() == Piece::Type::King) {
                const SDL_Point viewKing = boardToViewSquare(piece->getX(), piece->getY());
                drawRedSquare(static_cast<float>(viewKing.x * gSize), static_cast<float>(viewKing.y * gSize));
            }
        }
    }

    SDL_Texture* heldTexture = nullptr;
    SDL_Rect heldDst{0, 0, gSize, gSize};

    for (int viewY = 0; viewY < 8; ++viewY) {
        for (int viewX = 0; viewX < 8; ++viewX) {
            const SDL_Point boardSquare = viewToBoardSquare(viewX, viewY);
            const Piece* piece = board.getPiece(boardSquare.x, boardSquare.y);
            if (piece == nullptr) {
                continue;
            }

            if (piece == heldPiece && heldPiece != nullptr) {
                const int offset = gSize / 2;
                SDL_Point mouse = lastMouseLogical;
                if (mouse.x < 0 || mouse.y < 0) {
                    int mouseX = 0;
                    int mouseY = 0;
                    SDL_GetMouseState(&mouseX, &mouseY);
                    mouse = windowToLogical(mouseX, mouseY);
                }
                if (mouse.x >= 0 && mouse.y >= 0) {
                    heldTexture = heldPiece->getTexture();
                    heldDst.x = mouse.x - offset;
                    heldDst.y = mouse.y - offset;
                    drawYellowSquare(static_cast<float>(gSize * viewX), static_cast<float>(gSize * viewY));
                }
                continue;
            }

            if (heldPiece == nullptr && piece == recentPiece && recentPiece != nullptr) {
                drawYellowSquare(static_cast<float>(gSize * viewX), static_cast<float>(gSize * viewY));
            }

            SDL_Texture* texture = piece->getTexture();
            if (texture != nullptr) {
                SDL_Rect dst{viewX * gSize, viewY * gSize, gSize, gSize};
                SDL_RenderCopy(renderer, texture, nullptr, &dst);
            }
        }
    }

    const std::vector<Move> &playerMoves = board.isWhiteTurn() ? whiteMoves : blackMoves;
    if (heldPiece != nullptr) {
        for (const auto &m : playerMoves) {
            if (m.getMovingPiece() == heldPiece) {
                const SDL_Point viewSquare = boardToViewSquare(m.getNewX(), m.getNewY());
                const int centerX = static_cast<int>((viewSquare.x + 0.5f) * gSize);
                const int centerY = static_cast<int>((viewSquare.y + 0.5f) * gSize);
                drawFilledCircle(centerX, centerY, 15, moveHintColor);
            }
        }
    }
    else if (recentPiece != nullptr) {
        for (const auto &m : playerMoves) {
            if (m.getMovingPiece() == recentPiece) {
                const SDL_Point viewSquare = boardToViewSquare(m.getNewX(), m.getNewY());
                const int centerX = static_cast<int>((viewSquare.x + 0.5f) * gSize);
                const int centerY = static_cast<int>((viewSquare.y + 0.5f) * gSize);
                drawFilledCircle(centerX, centerY, 15, moveHintColor);
            }
        }
    }

    if (heldPiece != nullptr) {
        SDL_Point mouse = lastMouseLogical;
        if (mouse.x < 0 || mouse.y < 0) {
            int mouseX = 0;
            int mouseY = 0;
            SDL_GetMouseState(&mouseX, &mouseY);
            mouse = windowToLogical(mouseX, mouseY);
        }

        if (mouse.x >= 0 && mouse.y >= 0) {
            const int xCord = mouse.x / gSize * gSize;
            const int yCord = mouse.y / gSize * gSize;
            drawHighlightSquare(xCord, yCord, gSize);
        }
    }

    if (heldTexture != nullptr) {
        SDL_RenderCopy(renderer, heldTexture, nullptr, &heldDst);
    }

    SDL_RenderPresent(renderer);
}

void Game::checkGameOver()
{
    const std::vector<Move> &playerMoves = board.isWhiteTurn() ? whiteMoves : blackMoves;
    if (playerMoves.empty()) {
        if (inCheck) {
            std::cout << (board.isWhiteTurn() ? "White" : "Black") << " is in checkmate";
        }
        else {
            std::cout << (board.isWhiteTurn() ? "White" : "Black") << " is in stalemate";
        }
        gameOver = true;
    }
}

void Game::getMoves()
{
    inCheck = false;
    std::vector<Move> &playerMoves = board.isWhiteTurn() ? whiteMoves : blackMoves;
    const std::vector<const Piece *> &playerPieces = board.isWhiteTurn() ? board.getWhitePieces() : board.getBlackPieces();

    playerMoves.clear();
    for (const auto piece : playerPieces) {
        if (!piece->isDead()) {
            piece->getPossibleMoves(playerMoves, board);
        }
    }

    const King* king = nullptr;
    for (const auto piece : playerPieces) {
        if (piece->getPieceType() == Piece::Type::King) {
            king = dynamic_cast<const King *>(piece);
        }
    }

    const auto it = std::remove_if(playerMoves.begin(), playerMoves.end(), [this, &king](const Move &m)
    {
        board.makeMove(m);
        if (king != nullptr && king->inCheck(board)) {
            board.unmakeMove(m);
            return true;
        }
        board.unmakeMove(m);
        return false;
    });

    playerMoves.erase(it, playerMoves.end());

    const auto &pieces = board.isWhiteTurn() ? board.getWhitePieces() : board.getBlackPieces();
    for (const auto piece : pieces) {
        if (piece->getPieceType() == Piece::Type::King) {
            const auto *k = dynamic_cast<const King *>(piece);
            if (k != nullptr && k->inCheck(board)) {
                inCheck = true;
            }
        }
    }

    checkGameOver();
}

bool Game::canMove(Move &m) const
{
    const std::vector<Move> &possibleMoves = board.isWhiteTurn() ? whiteMoves : blackMoves;

    for (const Move &move : possibleMoves) {
        if (m == move) {
            if (move.getMoveType() == Move::MoveType::Castle || move.getMoveType() == Move::MoveType::Promotion) {
                m = move;
            }
            return true;
        }
    }

    return false;
}

void Game::reset()
{
#ifdef __EMSCRIPTEN__
    menuActive = true;
    aiStarted = false;
    aiIsWhite = false;
#else
    drawUi();
#endif
    board.decipherFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    gameOver = false;
    aiPending = false;
    heldPiece = nullptr;
    recentPiece = nullptr;
    inCheck = false;
    board.setWhiteTurn(true);
    whiteMoves.clear();
    blackMoves.clear();
    playedMoves.clear();
    playedMoves.reserve(10);
    getMoves();
}

void Game::basicWindowM(std::future<void> &f)
{
    while (f.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                SDL_DestroyWindow(window);
                window = nullptr;
            }
            else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                enforceSquareWindow(event.window.data1, event.window.data2);
            }
        }
    }
}

void Game::drawUi()
{
    menuActive = true;
    while (window != nullptr && menuActive) {
        drawUiFrame();
    }
    SDL_SetCursor(arrow);
}

void Game::drawUiFrame()
{
    if (window == nullptr || renderer == nullptr) {
        return;
    }

    const SDL_Color lightButton{240, 217, 181, 255};
    const SDL_Color darkButton{181, 136, 99, 255};
    const SDL_Color blackButton{0, 0, 0, 255};

    Button aiWhite(lightButton, logicalSize / 2 - 100, 100, 200, 100);
    Button aiBlack(darkButton, logicalSize / 2 - 100, 300, 200, 100);
    Button noAi(blackButton, logicalSize / 2 - 100, 500, 200, 100);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            SDL_DestroyWindow(window);
            window = nullptr;
        }
        else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
            enforceSquareWindow(event.window.data1, event.window.data2);
        }
        else if (event.type == SDL_MOUSEMOTION) {
            lastMouseLogical = eventToLogical(event.motion.x, event.motion.y);
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN) {
            const SDL_Point click = eventToLogical(event.button.x, event.button.y);
            lastMouseLogical = click;
            logClickDebug("ui-down", event.button.x, event.button.y, click, -1, -1);
            if (click.x >= 0 && click.y >= 0 && aiWhite.mouseOver(click.x, click.y)) {
                if (debugClicks) {
                    std::cout << "UI click: ai-white\n";
                }
                aiStarted = true;
                aiIsWhite = true;
                menuActive = false;
            }
            else if (click.x >= 0 && click.y >= 0 && aiBlack.mouseOver(click.x, click.y)) {
                if (debugClicks) {
                    std::cout << "UI click: ai-black\n";
                }
                aiStarted = true;
                aiIsWhite = false;
                menuActive = false;
            }
            else if (click.x >= 0 && click.y >= 0 && noAi.mouseOver(click.x, click.y)) {
                if (debugClicks) {
                    std::cout << "UI click: no-ai\n";
                }
                aiStarted = false;
                aiIsWhite = false;
                menuActive = false;
            }
            else if (debugClicks) {
                std::cout << "UI click: none\n";
            }
        }
    }

    SDL_Point mouse = lastMouseLogical;
    if (mouse.x < 0 || mouse.y < 0) {
        int mouseX = 0;
        int mouseY = 0;
        SDL_GetMouseState(&mouseX, &mouseY);
        mouse = windowToLogical(mouseX, mouseY);
    }
    const int mouseX = mouse.x;
    const int mouseY = mouse.y;

    const bool hoverWhite = aiWhite.mouseOver(mouseX, mouseY);
    const bool hoverBlack = aiBlack.mouseOver(mouseX, mouseY);
    const bool hoverNoAi = noAi.mouseOver(mouseX, mouseY);

    if (hoverWhite || hoverBlack || hoverNoAi) {
        SDL_SetCursor(click != nullptr ? click : arrow);
    }
    else {
        SDL_SetCursor(arrow);
    }

    aiWhite.draw(renderer);
    aiBlack.draw(renderer);
    noAi.draw(renderer);

    const SDL_Color textBlack{0, 0, 0, 255};
    const SDL_Color textWhite{255, 255, 255, 255};

    const std::string chooseText = "Choose Ai Color";
    const SDL_Point chooseSize = measureText(chooseText);
    const int chooseX = logicalSize / 2 - chooseSize.x / 2;
    const int chooseY = logicalSize / 20;
    drawText(chooseText, chooseX, chooseY, textBlack);

    const std::string whiteText = "White";
    const SDL_Point whiteSize = measureText(whiteText);
    const SDL_Rect whiteRect = aiWhite.getRect();
    const int whiteX = whiteRect.x + whiteRect.w / 2 - whiteSize.x / 2;
    const int whiteY = whiteRect.y + whiteRect.h / 2 - whiteSize.y / 2;
    drawText(whiteText, whiteX, whiteY, textBlack);

    const std::string blackText = "Black";
    const SDL_Point blackSize = measureText(blackText);
    const SDL_Rect blackRect = aiBlack.getRect();
    const int blackX = blackRect.x + blackRect.w / 2 - blackSize.x / 2;
    const int blackY = blackRect.y + blackRect.h / 2 - blackSize.y / 2;
    drawText(blackText, blackX, blackY, textBlack);

    const std::string noAiText = "No Ai";
    const SDL_Point noAiSize = measureText(noAiText);
    const SDL_Rect noAiRect = noAi.getRect();
    const int noAiX = noAiRect.x + noAiRect.w / 2 - noAiSize.x / 2;
    const int noAiY = noAiRect.y + noAiRect.h / 2 - noAiSize.y / 2;
    drawText(noAiText, noAiX, noAiY, textWhite);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 220);
    if (hoverWhite) {
        SDL_Rect r = aiWhite.getRect();
        SDL_RenderDrawRect(renderer, &r);
    }
    if (hoverBlack) {
        SDL_Rect r = aiBlack.getRect();
        SDL_RenderDrawRect(renderer, &r);
    }
    if (hoverNoAi) {
        SDL_Rect r = noAi.getRect();
        SDL_RenderDrawRect(renderer, &r);
    }

    SDL_RenderPresent(renderer);

    if (!menuActive) {
        SDL_SetCursor(arrow);
    }
}
