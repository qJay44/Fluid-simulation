#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Fluid.h"

void Init();
void Draw(Fluid& fluid, sf::Window& win, float scale);
const float Normalize(const float val, const float minIn, const float maxIn, const float minOut, const float maxOut);
void SetIcon(sf::Window& wnd);

int main()
{
    sf::Window window(sf::VideoMode(600, 600), "My OpenGL window", sf::Style::Close);
    SetIcon(window);

    window.setVerticalSyncEnabled(true);
    window.setActive(true);

    Init();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    const unsigned int w = window.getSize().x;
    const unsigned int h = window.getSize().y;
    const float scale = 0.0001f;

    Fluid fluid(0.1f, 0.f, 0.f, scale, w * h);

	// current mouse coords
	int MouseX = 0;
	int MouseY = 0;

	// previous mouse coords
	int MouseX0 = 0;
	int MouseY0 = 0;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && sf::Keyboard::Space))
                window.close();

            if (event.type == sf::Event::Resized)
                glViewport(0, 0, event.size.width, event.size.height);

			if (event.type == sf::Event::MouseMoved)
			{
				MouseX = sf::Mouse::getPosition(window).x;
				MouseY = sf::Mouse::getPosition(window).y;
			}

			// add density and velocity when mouse pressed
			if (event.type == sf::Event::MouseMoved && sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				const int x = MouseX / scale;
				const int y = MouseY / scale;

				const int x0 = MouseX0 / scale;
				const int y0 = MouseY0 / scale;

				const float velX = (float)x - x0;
				const float velY = (float)y - y0;

				fluid.addDensity(x, y, 300.f);
				fluid.addVelocity(x, y, velX, velY);
			}

			MouseX0 = MouseX;
			MouseY0 = MouseY;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        fluid.step();
		Draw(fluid, window, scale);
        fluid.fadeDensity();

		window.display();

    }

    return 0;
}

void Init()
{
    glClearColor(0.f, 0.f, 0.0f, 1.0f);
}

void Draw(Fluid& fluid, sf::Window& win, float scale)
{
	glLoadIdentity();

    const unsigned int w = win.getSize().x;
    const unsigned int h = win.getSize().y;
	const float size = Normalize(scale, 0.f, w * h, 0.f, 4.f);

    for (size_t i = 0; i < w; i++)
    {
        for (size_t j = 0; j < h; j++)
        {
			const float density = fluid.getDensity(i, j);

            const float posX = Normalize((float)i, 0.f, w - 1, -1.f, 1.f);
            const float posY = Normalize((float)j, 0.f, h - 1, -1.f, 1.f);

			glBegin(GL_QUADS);
			//glColor4f(1.f, 1.f, 1.f, (GLfloat) density / 100.f);
			glColor4f(1.f, 1.f, 1.f, 0.2f);

            glVertex2f(posX, -posY + size);
            glVertex2f(posX, -posY);
            glVertex2f(posX + size, -posY);
            glVertex2f(posX + size, -posY + size);

			glEnd();
        }
    }

	glFlush();
}

// normalize values
const float Normalize(const float val, const float minIn, const float maxIn, const float minOut, const float maxOut)
{
	const float x = (val - minIn) / (maxIn - minIn);
	const float result = minOut + (maxOut - minOut) * x;

	return (result < minOut) ? minOut : (result > maxOut) ? maxOut : result;
}

void SetIcon(sf::Window& wnd)
{
    sf::Image image;

    image.create(16, 16);
    for (int i = 0; i < 16; ++i)
        for (int j = 0; j < 16; ++j)
            image.setPixel(i, j, {
                (sf::Uint8) (i * 16), (sf::Uint8)(j * 16), 0 });

    wnd.setIcon(image.getSize().x, image.getSize().y, image.getPixelsPtr());
}
