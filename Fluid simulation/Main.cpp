#include <iostream>
#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include "SFML/System.hpp"

#include "Constants.h"
#include "Fluid.h"

void renderDensity(Fluid&, sf::RenderWindow&);
const int pIX(int x, int y);

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

			if (event.type == sf::Event::MouseMoved)
			{
				const int x = sf::Mouse::getPosition(window).x / (SCALE);
				const int y = sf::Mouse::getPosition(window).y / (SCALE);

				const float velX = (float) x - prev_x;
				const float velY = (float) y - prev_y;

				fluid.addDensity(x, y, 100.f);
				fluid.addVelocity(x, y, velX, velY);

				prev_x = x;
				prev_y = y;
			}
				
		}

		window.clear();

		fluid.step();
		renderDensity(fluid, window);

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
			const int x = i * SCALE;
			const int y = j * SCALE;
			const sf::Uint8 d = (sf::Uint8) fluid.getDensity()[pIX(i, j)];

			sf::RectangleShape rect;
			rect.setFillColor(sf::Color(d, d, d));
			rect.setSize(sf::Vector2f(SCALE, SCALE));
			rect.setPosition(sf::Vector2f((float) x, (float) y));

			win.draw(rect);
		}
	}
}

const int pIX(int x, int y)
{
	x = x < 0 ? 0 : x;
	x = x > N - 1 ? N - 1 : x;

	y = y < 0 ? 0 : y;
	y = y > N - 1 ? N - 1 : y;

	return x + (y * N);
}
