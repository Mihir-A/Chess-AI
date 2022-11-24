#pragma once
#include <SFML/Graphics.hpp>

class Button
{
public:
    Button(const sf::Color&, int x, int y, int width, int height);
    bool mouseOver(int x, int y) const;
    void draw(sf::RenderWindow& w);
private:
    sf::RectangleShape rect;
};

