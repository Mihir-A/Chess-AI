#include "Button.h"

Button::Button(const sf::Color &fillColor, float x, float y, float width, float height)
    : rect(sf::Vector2f(width, height))
{
    rect.setPosition(x, y);
    rect.setFillColor(fillColor);
}

bool Button::mouseOver(float x, float y) const
{
    return (rect.getGlobalBounds().contains(x, y));
}

void Button::draw(sf::RenderWindow &w)
{
    w.draw(rect);
}

const sf::RectangleShape& Button::getRect() const
{
    return rect;
}
