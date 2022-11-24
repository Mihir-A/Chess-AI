#include "Button.h"

Button::Button(const sf::Color& fillColor, int x, int y, int width, int height)
    : rect(sf::Vector2f(width, height))
{
    rect.setPosition(x, y);
    rect.setFillColor(fillColor);
}

bool Button::mouseOver(int x, int y) const
{
    return (rect.getGlobalBounds().contains(x, y));
}

void Button::draw(sf::RenderWindow& w)
{
    w.draw(rect);
}


