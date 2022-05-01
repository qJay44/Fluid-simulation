#include <iostream>
#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include "SFML/System.hpp"

#include "Constants.h"
#include "Fluid.h"

void renderDensity(Fluid&, sf::RenderWindow&);

int main()
{
	const unsigned int w = N * SCALE;
	const unsigned int h = N * SCALE;

	sf::RenderWindow window(sf::VideoMode(w, h), "Fluid simulation", sf::Style::Default);
	window.setFramerateLimit(75);

	Fluid fluid(0.1f, 0, 0);

	int prev_x = 0;
	int prev_y = 0;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Key::C)
				fluid.changeColorMode();

			if (event.type == sf::Event::MouseMoved)
			{
				const int x = sf::Mouse::getPosition(window).x / SCALE;
				const int y = sf::Mouse::getPosition(window).y / SCALE;

				const float velX = (float) x - prev_x;
				const float velY = (float) y - prev_y;

				fluid.addDensity(x, y, 300.f);
				fluid.addVelocity(x, y, velX, velY);

				prev_x = x;
				prev_y = y;
			}
		}

		window.clear();

		fluid.step();
		renderDensity(fluid, window);
		fluid.fadeDensity();

		window.display();
	}

	return 0;
}

void renderDensity(Fluid& fluid, sf::RenderWindow& win)
{
	for (size_t i = 0; i < N; i++)
	{
		for (size_t j = 0; j < N; j++)
		{
			sf::RectangleShape rect;
			rect.setSize(sf::Vector2f(SCALE, SCALE));
			rect.setPosition(i * SCALE, j * SCALE);

			switch (fluid.getColorMode())
			{
			case 0:
				const float density = fluid.getDensity(i, j);
				const sf::Uint8 alpha = density > 255 ? (sf::Uint8) 255 : (sf::Uint8) density;
				rect.setFillColor(sf::Color(255, 255, 255, alpha));

				break;
			case 1:
				rect.setFillColor()

			default:
				break;
			}

			win.draw(rect);
		}
	}
}

