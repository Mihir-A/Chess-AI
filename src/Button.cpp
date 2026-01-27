#include "Button.h"

Button::Button(const SDL_Color &fillColor, int x, int y, int width, int height)
    : fillColor(fillColor)
{
    rect.x = x;
    rect.y = y;
    rect.w = width;
    rect.h = height;
}

bool Button::mouseOver(int x, int y) const
{
    return x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h;
}

void Button::draw(SDL_Renderer* renderer) const
{
    SDL_SetRenderDrawColor(renderer, fillColor.r, fillColor.g, fillColor.b, fillColor.a);
    SDL_RenderFillRect(renderer, &rect);
}

const SDL_Rect& Button::getRect() const
{
    return rect;
}
