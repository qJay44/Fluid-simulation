#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Constants.h"
#include "Fluid.h"

void SetIcon(sf::Window& wnd);
void Init();
void Draw(Fluid& fluid, sf::Window& win);
const float Normalize(const float val, const float minIn, const float maxIn, const float minOut, const float maxOut);

int main() {

	const unsigned int w = N * SCALE;
	const unsigned int h = N * SCALE;


    sf::Window window(sf::VideoMode(w, h), "My OpenGL window", sf::Style::Default);
    SetIcon(window);

    window.setVerticalSyncEnabled(true);
    window.setActive(true);

    Init();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );

    Fluid fluid(0.1f, 0.f, 0.f);

	// current mouse coords
	int MouseX = 0;
	int MouseY = 0;

	// previous mouse coords
	int MouseX0 = 0;
	int MouseY0 = 0;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::Resized) {
                glViewport(0, 0, event.size.width, event.size.height);
            }

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

				fluid.addDensity(x, y, 100.f);
				fluid.addVelocity(x, y, velX, velY);
			}

			MouseX0 = MouseX;
			MouseY0 = MouseY;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        fluid.step();
		Draw(fluid, window);
        fluid.fadeDensity();

		window.display();

    }

    return 0;
}

void Init()
{
    glClearColor(0.f, 0.f, 0.0f, 1.0f);
}

void Draw(Fluid& fluid, sf::Window& win)
{
	glLoadIdentity();
    for (size_t i = 0; i < N; i++)
    {
        for (size_t j = 0; j < N; j++)
        {
            const float width = win.getSize().x;
            const float height = win.getSize().y;
			const float density = fluid.getDensity(i, j);

            const float posX = Normalize((i * SCALE), 0.f, (N - 1) * SCALE, -1.f, 1.f);
            const float posY = Normalize((j * SCALE), 0.f, (N - 1) * SCALE, -1.f, 1.f);
            const float size = 2.f / SCALE;

			glBegin(GL_QUADS);
			glColor4f(1.f, 1.f, 1.f, (GLfloat) density / 100.f);

            glVertex2f(posX, -posY);
            glVertex2f(posX, -posY + size);
            glVertex2f(posX + size, -posY + size);
            glVertex2f(posX + size, -posY);

			glEnd();
        }
    }
	glFlush();
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

// normalize values
const float Normalize(const float val, const float minIn, const float maxIn, const float minOut, const float maxOut)
{
	const float x = (val - minIn) / (maxIn - minIn);
	const float result = minOut + (maxOut - minOut) * x;

	return (result < minOut) ? minOut : (result > maxOut) ? maxOut : result;
}