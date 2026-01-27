#pragma once

#include <SDL.h>

class Button
{
public:
    Button(const SDL_Color &fillColor, int x, int y, int width, int height);
    bool mouseOver(int x, int y) const;
    void draw(SDL_Renderer* renderer) const;
    const SDL_Rect& getRect() const;

private:
    SDL_Rect rect{};
    SDL_Color fillColor{};
};
