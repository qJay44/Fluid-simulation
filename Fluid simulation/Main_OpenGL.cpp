#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Fluid.h"

void Init();
void Draw(Fluid& fluid, sf::Window& win, const unsigned int N, const unsigned int SCALE);
void HSV(int hue, float sat, float va, const float d);
const float Normalize(const float val, const float minIn, const float maxIn, const float minOut, const float maxOut);
void SetIcon(sf::Window& wnd);

int main()
{
    Fluid fluid(0.1f, 0.f, 0.f);

    const unsigned int N = fluid.getN();
    const unsigned int SCALE = fluid.getScale();

    sf::Window window(sf::VideoMode(N * SCALE, N * SCALE), "My OpenGL window", sf::Style::Close);
    SetIcon(window);

    window.setVerticalSyncEnabled(true);
    window.setActive(true);

    Init();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

	// current mouse coords
	int MouseX = 0;
	int MouseY = 0;

	// previous mouse coords
	int MouseX0 = 0;
	int MouseY0 = 0;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Space))
                window.close();

            if (event.type == sf::Event::Resized)
                glViewport(0, 0, event.size.width, event.size.height);

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

				fluid.addDensity(x, y, 300.f);
				fluid.addVelocity(x, y, velX, velY);
			}

			MouseX0 = MouseX;
			MouseY0 = MouseY;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        fluid.step();
		Draw(fluid, window, N, SCALE);
        fluid.fadeDensity();

		window.display();
    }

    return 0;
}

struct { GLfloat colors[4] = {0.f, 0.f, 0.f, 0.f}; } ColorVar;

void Init()
{
    glClearColor(0.f, 0.f, 0.0f, 1.0f);
}

void Draw(Fluid& fluid, sf::Window& win, const unsigned int N, const unsigned int SCALE)
{
	glLoadIdentity();

	const float size = Normalize((float)SCALE, 0.f, (float)N * SCALE, 0.f, 2.f);

    for (size_t i = 0; i < N; i++)
    {
        for (size_t j = 0; j < N; j++)
        {
			const float density = fluid.getDensity(i, j);
            const float posX = Normalize((float)i, 0.f, N - 1.f, -1.f, 1.f);
            const float posY = Normalize((float)j, 0.f, N - 1.f, -1.f, 1.f);

            switch (fluid.getColorMode())
            {
				case 0:
				{
					ColorVar.colors[0] = 1.f;
					ColorVar.colors[1] = 1.f;
					ColorVar.colors[2] = 1.f;
					ColorVar.colors[3] = density / 100.f;
					break;
				}

				case 1:
				{
					HSV((int)density, 1.f, 1.f, 1.f);
					break;
				}

				case 2:
				{
					ColorVar.colors[0] = Normalize(fluid.getVelX(i, j), -0.05f, 0.05f, 0.f, 1.f);
					ColorVar.colors[1] = Normalize(fluid.getVelY(i, j), -0.05f, 0.05f, 0.f, 1.f);
					ColorVar.colors[2] = 1.f;
					ColorVar.colors[3] = 1.f;
					break;
				}
				default:
					break;
            }

			glBegin(GL_QUADS);

			glColor4f(ColorVar.colors[0], ColorVar.colors[1], ColorVar.colors[2], ColorVar.colors[3]);
            glVertex2f(posX, -posY + size);
            glVertex2f(posX, -posY);
            glVertex2f(posX + size, -posY);
            glVertex2f(posX + size, -posY + size);

			glEnd();
        }
    }

	glFlush();
}

void HSV(int hue, float sat, float val, const float d)
{
	hue %= 360;
	while (hue < 0) hue += 360;

	if (sat < 0.f) sat = 0.f;
	if (sat > 1.f) sat = 1.f;

	if (val < 0.f) val = 0.f;
	if (val > 1.f) val = 1.f;

	const int h = hue / 60;
	float f = float(hue) / 60.f - h;
	float p = val * (1.f - sat);
	float q = val * (1.f - sat * f);
	float t = val * (1.f - sat * (1.f - f));

	switch (h)
	{
		default:
		case 0:
		case 6: { ColorVar.colors[0] = val * 1.f; ColorVar.colors[1] = t * 1.f;   ColorVar.colors[2] = p * 1.f;   ColorVar.colors[3] = d; break; }
		case 1: { ColorVar.colors[0] = q * 1.f;   ColorVar.colors[1] = val * 1.f, ColorVar.colors[2] = p * 1.f,   ColorVar.colors[3] = d; break; }
		case 2: { ColorVar.colors[0] = p * 1.f;   ColorVar.colors[1] = val * 1.f, ColorVar.colors[2] = t * 1.f,   ColorVar.colors[3] = d; break; }
		case 3: { ColorVar.colors[0] = p * 1.f;   ColorVar.colors[1] = q * 1.f,   ColorVar.colors[2] = val * 1.f, ColorVar.colors[3] = d; break; }
		case 4: { ColorVar.colors[0] = t * 1.f;   ColorVar.colors[1] = p * 1.f,   ColorVar.colors[2] = val * 1.f, ColorVar.colors[3] = d; break; }
		case 5: { ColorVar.colors[0] = val * 1.f; ColorVar.colors[1] = p * 1.f,   ColorVar.colors[2] = q * 1.f,   ColorVar.colors[3] = d; break; }
	}
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
