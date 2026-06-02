#include <SFML/Graphics.hpp>
#include <iostream>
using namespace std;
using namespace sf;
int main()
{
    float windowWidth = 600.f;
    float windowHeight = 600.f;

    RenderWindow window(sf::VideoMode({ 600, 600 }),"Chess");

    while (window.isOpen())
    {
        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        window.clear(Color(127,127,127));

        

        float sqSize = windowHeight / 8;
        for (int i = 0; i < 8;i++)
        {
            for (int j = 0; j < 8; j++)
            {
                RectangleShape rect({ sqSize,sqSize });
                rect.setPosition({(sqSize * i),(sqSize * j) });
                if ((i+j) % 2 == 0)
                {
                    rect.setFillColor(Color(245, 231, 185));
                }
                else
                {
                    rect.setFillColor(Color(141, 96, 37));
                }

                window.draw(rect);
            }
        }
        window.display();
    }
}