#pragma once
#include <SFML/Graphics.hpp>

class Button
{
public:
    Button(const sf::Color &, float x, float y, float width, float height);
    bool mouseOver(float x, float y) const;
    void draw(sf::RenderWindow &w);
    const sf::RectangleShape& getRect() const;
private:
    sf::RectangleShape rect;
};
