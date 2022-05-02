#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include "SFML/System.hpp"

#include "Constants.h"
#include "Fluid.h"
#include <iostream>

void renderDensity(Fluid&, sf::RenderWindow&);
const sf::Color HSV(int hue, float sat, float va, const float d);
const float MapToRange(const float val, const float minIn, const float maxIn, const float minOut, const float maxOut);

int main()
{
	const unsigned int w = N * SCALE;
	const unsigned int h = N * SCALE;

	sf::RenderWindow window(sf::VideoMode(w, h), "Fluid simulation", sf::Style::Close);
	window.setFramerateLimit(75);

	Fluid fluid(0.1f, 0.f, 0.f);

	// current mouse coords
	int MouseX = 0;
	int MouseY = 0;

	// previous mouse coords
	int MouseX0 = 0;
	int MouseY0 = 0;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			// change color mode
			if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Key::C)
				fluid.changeColorMode();

			if (event.type == sf::Event::MouseMoved)
			{
				MouseX = sf::Mouse::getPosition(window).x;
				MouseY = sf::Mouse::getPosition(window).y;
			}

			// add density and velocity when mouse pressed
			if (event.type == sf::Event::MouseMoved && sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				const int x = MouseX / SCALE;
				const int y = MouseY / SCALE;

				const int x0 = MouseX0 / SCALE;
				const int y0 = MouseY0 / SCALE;

				const float velX = (float)x - x0;
				const float velY = (float)y - y0;

				fluid.addDensity(x, y, 500.f);
				fluid.addVelocity(x, y, velX, velY);
			}

			MouseX0 = MouseX;
			MouseY0 = MouseY;
		}

		window.clear();

		// draw
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
			rect.setPosition((float) i * SCALE, (float) j * SCALE);
			const float density = fluid.getDensity(i, j);

			switch (fluid.getColorMode())
			{
				// Black and white color mode
				case 0:
				{
					const sf::Uint8 alpha = density > 255 ? (sf::Uint8)255 : (sf::Uint8)density;
					rect.setFillColor(sf::Color(255, 255, 255, alpha));

					break;
				}
				// HSV color mode
				case 1:
					rect.setFillColor(HSV((int) density, 1, 1, 255));
					break;
				// HSV velocity color mode
				case 2:
				{
					const unsigned int r = (int)MapToRange(fluid.getVelX(i, j), -0.05f, 0.05f, 0, 255);
					const unsigned int g = (int)MapToRange(fluid.getVelY(i, j), -0.05f, 0.05f, 0, 255);
					rect.setFillColor(sf::Color(r, g, 255));
					break;
				}
				default:
					break;
			};

			win.draw(rect);
		}
	}
}

const sf::Color HSV(int hue, float sat, float val, const float d)
{
	hue %= 360;
	while (hue < 0) hue += 360;

	if (sat < 0.f) sat = 0.f;
	if (sat > 1.f) sat = 1.f;

	if (val < 0.f) val = 0.f;
	if (val > 1.f) val = 1.f;

	const int h = hue / 60;
	float f = float(hue) / 60 - h;
	float p = val * (1.f - sat);
	float q = val * (1.f - sat * f);
	float t = val * (1.f - sat * (1 - f));

	switch (h)
	{
		default:
		case 0:
		case 6: return sf::Color(val * 255, t * 255, p * 255, d);
		case 1: return sf::Color(q * 255, val * 255, p * 255, d);
		case 2: return sf::Color(p * 255, val * 255, t * 255, d);
		case 3: return sf::Color(p * 255, q * 255, val * 255, d);
		case 4: return sf::Color(t * 255, p * 255, val * 255, d);
		case 5: return sf::Color(val * 255, p * 255, q * 255, d);
	}
}

// normalize values
const float MapToRange(const float val, const float minIn, const float maxIn, const float minOut, const float maxOut)
{
	const float x = (val - minIn) / (maxIn - minIn);
	const float result = minOut + (maxOut - minOut) * x;

	return (result < minOut) ? minOut : (result > maxOut) ? maxOut : result;
}
