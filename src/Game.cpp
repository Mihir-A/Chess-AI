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
    while (tick()) {
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
        else if (gameOver && resultVisible && handleResultEvent(event)) {
            // The result dialog owns input until dismissed.
            if (menuActive) {
                return true;
            }
        }
        else if (gameOver && (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP)) {
            // Viewing the final position must not select or move pieces.
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
            resultVisible = false;
        }
        else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r) {
#ifdef __EMSCRIPTEN__
            if ((event.key.keysym.mod & (KMOD_CTRL | KMOD_GUI)) != 0) {
                continue;
            }
#endif
            reset();
            return true;
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

SDL_Point Game::measureText(const std::string &text, float pixelHeight) const
{
    SDL_Point size{0, 0};
    if (!fontLoaded || text.empty()) {
        return size;
    }

    const float textScale = stbtt_ScaleForPixelHeight(&fontInfo, pixelHeight);
    float width = 0.0f;
    int prev = 0;
    for (unsigned char ch : text) {
        const int codepoint = static_cast<int>(ch);
        if (prev != 0) {
            width += stbtt_GetCodepointKernAdvance(&fontInfo, prev, codepoint) * textScale;
        }
        int advance = 0;
        int leftBearing = 0;
        stbtt_GetCodepointHMetrics(&fontInfo, codepoint, &advance, &leftBearing);
        width += advance * textScale;
        prev = codepoint;
    }

    const float height = static_cast<float>(fontAscent - fontDescent + fontLineGap) * textScale;
    size.x = static_cast<int>(std::ceil(width));
    size.y = static_cast<int>(height);
    return size;
}

void Game::drawText(const std::string &text, int x, int y, const SDL_Color &color, float pixelHeight) const
{
    if (!fontLoaded || renderer == nullptr || text.empty()) {
        return;
    }

    const float textScale = stbtt_ScaleForPixelHeight(&fontInfo, pixelHeight);
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    SDL_RenderGetScale(renderer, &scaleX, &scaleY);
    // Rasterize at display resolution, with 2x supersampling for smaller windows.
    const float rasterScale = std::max(2.0f, std::max(scaleX, scaleY));
    const float bitmapScale = textScale * rasterScale;
    float penX = static_cast<float>(x);
    const float baseline = y + fontAscent * textScale;
    int prev = 0;

    for (unsigned char ch : text) {
        const int codepoint = static_cast<int>(ch);

        if (prev != 0) {
            penX += stbtt_GetCodepointKernAdvance(&fontInfo, prev, codepoint) * textScale;
        }

        int advance = 0;
        int leftBearing = 0;
        stbtt_GetCodepointHMetrics(&fontInfo, codepoint, &advance, &leftBearing);

        int w = 0;
        int h = 0;
        int xoff = 0;
        int yoff = 0;
        unsigned char* bitmap = stbtt_GetCodepointBitmap(&fontInfo, 0.0f, bitmapScale, codepoint, &w, &h, &xoff, &yoff);

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
                    SDL_SetTextureScaleMode(texture, SDL_ScaleModeLinear);
                    const SDL_FRect dst{penX + xoff / rasterScale, baseline + yoff / rasterScale,
                                        w / rasterScale, h / rasterScale};
                    SDL_RenderCopyF(renderer, texture, nullptr, &dst);
                    SDL_DestroyTexture(texture);
                }
            }
            stbtt_FreeBitmap(bitmap, nullptr);
        }

        penX += advance * textScale;
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

    if (gameOver && resultVisible) {
        drawGameResult();
    }
    SDL_RenderPresent(renderer);
}

void Game::checkGameOver()
{
    const std::vector<Move> &playerMoves = board.isWhiteTurn() ? whiteMoves : blackMoves;
    const bool wasGameOver = gameOver;
    gameOver = playerMoves.empty();
    if (gameOver && !wasGameOver) {
        resultVisible = true;
        resultFocus = 0;
        heldPiece = nullptr;
        recentPiece = nullptr;
    }
    else if (!gameOver) {
        resultVisible = false;
    }
}

bool Game::handleResultEvent(const SDL_Event &event)
{
    const SDL_Rect newGame{204, 438, 184, 60};
    const SDL_Rect viewBoard{412, 438, 184, 60};
    const auto activate = [&](int choice) {
        SDL_SetCursor(arrow);
        if (choice == 0) {
            reset();
        }
        else {
            resultVisible = false;
        }
    };
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == SDL_BUTTON_LEFT) {
            const SDL_Point mouse = eventToLogical(event.button.x, event.button.y);
            if (SDL_PointInRect(&mouse, &newGame)) {
                activate(0);
            }
            else if (SDL_PointInRect(&mouse, &viewBoard)) {
                activate(1);
            }
        }
        return true;
    }
    if (event.type == SDL_MOUSEBUTTONUP) {
        return true;
    }
    if (event.type == SDL_KEYDOWN) {
        if (event.key.repeat != 0) {
            return true;
        }
        switch (event.key.keysym.sym) {
        case SDLK_TAB:
        case SDLK_LEFT:
        case SDLK_RIGHT:
            resultFocus = 1 - resultFocus;
            return true;
        case SDLK_RETURN:
        case SDLK_KP_ENTER:
        case SDLK_SPACE:
            activate(resultFocus);
            return true;
        case SDLK_ESCAPE:
            activate(1);
            return true;
        default:
            break;
        }
    }
    return false;
}

void Game::drawGameResult()
{
    SDL_SetRenderDrawColor(renderer, 20, 18, 16, 155);
    SDL_Rect backdrop{0, 0, logicalSize, logicalSize};
    SDL_RenderFillRect(renderer, &backdrop);
    drawRoundedRect({164, 278, 472, 260}, 20, SDL_Color{43, 40, 36, 255});

    const SDL_Color primary{248, 240, 227, 255};
    const SDL_Color accent{225, 185, 125, 255};
    const std::string title = inCheck ? "Checkmate" : "Stalemate";
    const std::string result = inCheck ? (board.isWhiteTurn() ? "Black wins" : "White wins") : "Draw";
    drawText(title, 400 - measureText(title, 44).x / 2, 310, primary, 44);
    drawText(result, 400 - measureText(result, 28).x / 2, 372, accent, 28);

    const SDL_Rect buttons[] = {{204, 438, 184, 60}, {412, 438, 184, 60}};
    const char* labels[] = {"New Game", "View Board"};
    bool hovering = false;
    for (int i = 0; i < 2; ++i) {
        const bool hovered = SDL_PointInRect(&lastMouseLogical, &buttons[i]);
        hovering = hovering || hovered;
        const bool focused = resultFocus == i || hovered;
        drawRoundedRect(buttons[i], 12, focused ? accent : SDL_Color{78, 71, 61, 255});
        SDL_Rect inside{buttons[i].x + 2, buttons[i].y + 2, buttons[i].w - 4, buttons[i].h - 4};
        drawRoundedRect(inside, 10, SDL_Color{56, 52, 46, 255});
        drawText(labels[i], buttons[i].x + (buttons[i].w - measureText(labels[i], 25).x) / 2,
                 buttons[i].y + 17, primary, 25);
    }
    SDL_SetCursor(hovering && click != nullptr ? click : arrow);
}

void Game::drawRoundedRect(SDL_Rect rect, int radius, SDL_Color color) const
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_Rect middle{rect.x + radius, rect.y, rect.w - 2 * radius, rect.h};
    SDL_Rect cross{rect.x, rect.y + radius, rect.w, rect.h - 2 * radius};
    SDL_RenderFillRect(renderer, &middle);
    SDL_RenderFillRect(renderer, &cross);
    drawFilledCircle(rect.x + radius, rect.y + radius, radius, color);
    drawFilledCircle(rect.x + rect.w - radius - 1, rect.y + radius, radius, color);
    drawFilledCircle(rect.x + radius, rect.y + rect.h - radius - 1, radius, color);
    drawFilledCircle(rect.x + rect.w - radius - 1, rect.y + rect.h - radius - 1, radius, color);
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
    menuFocus = 0;
    menuActive = true;
    aiStarted = false;
    aiIsWhite = false;
    resultVisible = false;
    resultFocus = 0;
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

void Game::drawUiFrame()
{
    if (window == nullptr || renderer == nullptr) {
        return;
    }

    const SDL_Color background{29, 27, 25, 255};
    const SDL_Color panel{43, 40, 36, 255};
    const SDL_Color card{56, 52, 46, 255};
    const SDL_Color activeCard{70, 62, 51, 255};
    const SDL_Color accent{225, 185, 125, 255};
    const SDL_Color primary{248, 240, 227, 255};
    const Button options[] = {
        Button(card, 152, 300, 236, 232),
        Button(card, 412, 300, 236, 232),
        Button(card, 152, 556, 496, 68)
    };
    const auto optionAt = [&](SDL_Point point) {
        for (int i = 0; i < 3; ++i) {
            if (options[i].mouseOver(point.x, point.y)) {
                return i;
            }
        }
        return -1;
    };
    const auto startGame = [&](int choice) {
        aiStarted = choice != 2;
        // The cards describe the player's side; the AI takes the opposite side.
        aiIsWhite = choice == 1;
        aiPending = false;
        menuActive = false;
        SDL_SetCursor(arrow);
    };

    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            SDL_DestroyWindow(window);
            window = nullptr;
            return;
        }
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
            enforceSquareWindow(event.window.data1, event.window.data2);
        }
        else if (event.type == SDL_MOUSEMOTION) {
            lastMouseLogical = eventToLogical(event.motion.x, event.motion.y);
            const int hovered = optionAt(lastMouseLogical);
            if (hovered >= 0) {
                menuFocus = hovered;
            }
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
            lastMouseLogical = eventToLogical(event.button.x, event.button.y);
            const int choice = optionAt(lastMouseLogical);
            if (choice >= 0) {
                startGame(choice);
                return;
            }
        }
        else if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
            const SDL_Keycode key = event.key.keysym.sym;
            if (key == SDLK_TAB) {
                menuFocus = (menuFocus + ((event.key.keysym.mod & KMOD_SHIFT) ? 2 : 1)) % 3;
            }
            else if (key == SDLK_RIGHT || key == SDLK_DOWN) {
                menuFocus = (menuFocus + 1) % 3;
            }
            else if (key == SDLK_LEFT || key == SDLK_UP) {
                menuFocus = (menuFocus + 2) % 3;
            }
            else if (key == SDLK_RETURN || key == SDLK_KP_ENTER || key == SDLK_SPACE) {
                startGame(menuFocus);
                return;
            }
        }
    }

    SDL_SetCursor(optionAt(lastMouseLogical) >= 0 && click != nullptr ? click : arrow);
    SDL_SetRenderDrawColor(renderer, background.r, background.g, background.b, 255);
    SDL_RenderClear(renderer);
    // A quiet board pattern connects the welcome screen to the game.
    SDL_SetRenderDrawColor(renderer, 34, 31, 28, 255);
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            if ((x + y) % 2 == 1) {
                SDL_Rect square{x * 100, y * 100, 100, 100};
                SDL_RenderFillRect(renderer, &square);
            }
        }
    }

    const auto centeredText = [&](const std::string &text, int centerX, int y, float size, SDL_Color color) {
        const SDL_Point bounds = measureText(text, size);
        drawText(text, centerX - bounds.x / 2, y, color, size);
    };

    drawRoundedRect({104, 134, 592, 524}, 24, panel);
    centeredText("CHESS AI", 400, 170, 21.0f, accent);
    centeredText("Choose your side", 400, 216, 48.0f, primary);

    for (int i = 0; i < 3; ++i) {
        SDL_Rect rect = options[i].getRect();
        const bool focused = menuFocus == i;
        drawRoundedRect(rect, 14, focused ? accent : SDL_Color{78, 71, 61, 255});
        SDL_Rect inset{rect.x + 2, rect.y + 2, rect.w - 4, rect.h - 4};
        drawRoundedRect(inset, 12, focused ? activeCard : card);
        if (i < 2) {
            const int centerX = rect.x + rect.w / 2;
            drawFilledCircle(centerX, 376, 49, SDL_Color{110, 94, 72, 255});
            // Reuse the same kings that appear on the board.
            const Piece* king = board.getPiece(4, i == 0 ? 7 : 0);
            if (king != nullptr && king->getTexture() != nullptr) {
                SDL_Rect icon{centerX - 47, 329, 94, 94};
                SDL_RenderCopy(renderer, king->getTexture(), nullptr, &icon);
            }
            centeredText(i == 0 ? "Play as White" : "Play as Black", centerX, 445, 30.0f, primary);
        }
        else {
            centeredText("Two players", rect.x + rect.w / 2, rect.y + 19, 27.0f, primary);
        }
    }
    SDL_RenderPresent(renderer);
}
