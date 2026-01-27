#include "Piece.h"

#include <array>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::unordered_map<std::string, SDL_Texture*> Piece::textures;

Piece::Piece(bool white, int x, int y, Type pieceType)
    : hasMoved(false)
      , white(white)
      , x(x)
      , y(y)
      , dead(false)
      , pieceType(pieceType)
{}

bool Piece::getHasMoved() const
{
    return hasMoved;
}

void Piece::setHasMoved(bool moved)
{
    hasMoved = moved;
}

bool Piece::isWhite() const
{
    return white;
}

SDL_Texture* Piece::getTexture() const
{
    std::string t = white ? "w" : "b";
    switch (pieceType) {
    case Type::Pawn:
        t += "p";
        break;
    case Type::Rook:
        t += "r";
        break;
    case Type::Queen:
        t += "q";
        break;
    case Type::King:
        t += "k";
        break;
    case Type::Bishop:
        t += "b";
        break;
    case Type::Knight:
        t += "n";
        break;
    }
    const auto it = textures.find(t);
    if (it == textures.end()) {
        return nullptr;
    }
    return it->second;
}

Piece::Type Piece::getPieceType() const
{
    return pieceType;
}

int Piece::getX() const
{
    return x;
}

int Piece::getY() const
{
    return y;
}

void Piece::moveTo(int xCord, int yCord)
{
    this->x = xCord;
    this->y = yCord;
    hasMoved = true;
}

bool Piece::isDead() const
{
    return dead;
}

void Piece::setKill(bool killed)
{
    dead = killed;
}

void Piece::setIsWhite(bool w)
{
    this->white = w;
}

bool Piece::onBoard(int p)
{
    return (p > -1 && p < 8);
}

void Piece::loadTextures(SDL_Renderer* renderer)
{
    if (texturesLoaded) {
        return;
    }
    if (renderer == nullptr) {
        std::cerr << "Cannot load textures without a renderer.\n";
        return;
    }

    const std::array<std::string, 12> texturesName{"wk", "bk", "wb", "bb", "wn", "bn", "wp", "bp", "wr", "br", "wq", "bq"};

    for (const auto &s : texturesName) {
        const std::string path = "assets/piece/" + s + ".png";
        int width = 0;
        int height = 0;
        int channels = 0;
        stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
        if (pixels == nullptr) {
            std::cerr << "Failed to load texture pixels: " << path << " (" << stbi_failure_reason() << ")\n";
            continue;
        }

        SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormatFrom(
            pixels,
            width,
            height,
            32,
            width * 4,
            SDL_PIXELFORMAT_RGBA32);
        if (surface == nullptr) {
            std::cerr << "Failed to create surface: " << path << " (" << SDL_GetError() << ")\n";
            stbi_image_free(pixels);
            continue;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        stbi_image_free(pixels);

        if (texture == nullptr) {
            std::cerr << "Failed to create texture: " << path << " (" << SDL_GetError() << ")\n";
            continue;
        }

        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        textures[s] = texture;
    }

    texturesLoaded = true;
}

void Piece::unloadTextures()
{
    for (auto &[name, texture] : textures) {
        if (texture != nullptr) {
            SDL_DestroyTexture(texture);
        }
        texture = nullptr;
    }
    textures.clear();
    texturesLoaded = false;
}
