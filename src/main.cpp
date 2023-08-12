#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include "SFML/System.hpp"
#include "Fluid.h"
#include "imgui.h"
#include "imgui-SFML.h"
#include <iostream>
#include <vector>


void renderDensity(Fluid&, sf::RenderWindow&, std::vector<sf::RectangleShape>&);
const sf::Color HSV(int hue, float sat, float va, const float d);
const float Normalize(const float val, const float minIn, const float maxIn, const float minOut, const float maxOut);

int main() {
  int densityLine = 3;
  float densityValue = 100.f;

  float dt = 0.1f;
  float diffusion = 1.e-4f;
  float viscocity = 0.f;

  auto resetValues = [&] () {
    densityLine = 3;
    densityValue = 100.f;

    dt = 0.1f;
    diffusion = 1.f;
    viscocity = 0.f;
  };

	Fluid fluid(dt, diffusion, viscocity);

	sf::RenderWindow window(sf::VideoMode(N * SCALE, N * SCALE), "Fluid simulation", sf::Style::Close);
	window.setFramerateLimit(75);
  if (!ImGui::SFML::Init(window))
    throw std::runtime_error("ImGui initialize fail");


  std::vector<sf::RectangleShape> grid(N * N);
  grid.reserve(N * N);


	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			sf::RectangleShape rect;
			rect.setSize(sf::Vector2f(SCALE, SCALE));
			rect.setPosition(i * SCALE, j * SCALE);
      grid[i + j * N] = rect;
    }
  }

	// current mouse coords
	int MouseX = 0;
	int MouseY = 0;

	// previous mouse coords
	int MouseX0 = 0;
	int MouseY0 = 0;

  bool doOnce = true;

  sf::Clock deltaClock;
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
      ImGui::SFML::ProcessEvent(event);

			if (event.type == sf::Event::Closed)
				window.close();

      if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Key::Q)
        window.close();

			// change color mode
			if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Key::C)
				fluid.changeColorMode();

			if (event.type == sf::Event::MouseMoved) {
				MouseX = sf::Mouse::getPosition(window).x;
				MouseY = sf::Mouse::getPosition(window).y;
			}

      // add density and velocity when mouse pressed
      if (event.type == sf::Event::MouseMoved && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        if (!ImGui::IsAnyItemHovered() && !ImGui::IsWindowHovered(ImGuiFocusedFlags_AnyWindow) && !ImGui::IsItemFocused()) {
          const int x = MouseX / SCALE;
          const int y = MouseY / SCALE;

          const int x0 = MouseX0 / SCALE;
          const int y0 = MouseY0 / SCALE;

          const float velX = (float)x - x0;
          const float velY = (float)y - y0;

          for (int i = -densityLine; i <= densityLine; i++) {
            fluid.addDensity(x + i, y, densityValue);
            fluid.addVelocity(x + i, y, velX, velY);
          }
        }
			}

			MouseX0 = MouseX;
			MouseY0 = MouseY;
		}

    ImGui::SFML::Update(window, deltaClock.restart());

    if (doOnce) {
      ImGui::SetNextWindowPos({ 0, 0 });
      ImGui::SetNextWindowCollapsed(true);

      doOnce = false;
    }

    ImGui::Begin("Settings");
    ImGui::SliderInt("add size", &densityLine, 0, 10);
    ImGui::SliderFloat("add density value", &densityValue, 0.f, 1000.f);

    ImGui::SliderFloat("time step", &dt, 0.001f, 3.f);
    ImGui::SliderFloat("diffusion", &diffusion, 1.e-5f, 0.01f, "%.5f");
    ImGui::SliderFloat("viscocity", &viscocity, 0.f, 1.f);

    if (ImGui::Button("Reset"))
      resetValues();

    ImGui::End();

		window.clear();

		// draw
		fluid.step();
		renderDensity(fluid, window, grid);
		fluid.fadeDensity();

    ImGui::SFML::Render(window);

		window.display();
	}

  ImGui::SFML::Shutdown();

	return 0;
}

void renderDensity(Fluid& fluid, sf::RenderWindow& win, std::vector<sf::RectangleShape>& grid) {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
      sf::RectangleShape& rect = grid[i + j * N];
			const float density = fluid.getDensity(i, j);

			switch (fluid.getColorMode()) {
				// Black and white color mode
				case 0: {
					const sf::Uint8 alpha = density > 255 ? (sf::Uint8)255 : (sf::Uint8)density;
					rect.setFillColor(sf::Color(255, 255, 255, alpha));

					break;
				}
				// HSV color mode
				case 1: {
					rect.setFillColor(HSV((int)density, 1.f, 1.f, 255.f));
					break;
				}
				// HSV velocity color mode
				case 2: {
					const unsigned int r = (int)Normalize(fluid.getVelX(i, j), -0.05f, 0.05f, 0, 255);
					const unsigned int g = (int)Normalize(fluid.getVelY(i, j), -0.05f, 0.05f, 0, 255);
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

const sf::Color HSV(int hue, float sat, float val, const float d) {
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

	switch (h) {
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
const float Normalize(const float val, const float minIn, const float maxIn, const float minOut, const float maxOut) {
	const float x = (val - minIn) / (maxIn - minIn);
	const float result = minOut + (maxOut - minOut) * x;

	return (result < minOut) ? minOut : (result > maxOut) ? maxOut : result;
}

