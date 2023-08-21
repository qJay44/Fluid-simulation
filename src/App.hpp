#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include "SFML/System.hpp"
#include "Fluid.hpp"
#include "imgui.h"
#include "imgui-SFML.h"
#include <iostream>
#include <string>
#include <vector>

class App {
  // default add properties
  int densityLine = 3;
  float densityValue = 100.f;

  // default fluid properties
  float dt = 0.1f;
  float diffusion = 1.e-4f;
  float viscocity = 0.f;

  // current mouse coords
  int MouseX = 0;
  int MouseY = 0;

  // previous mouse coords
  int MouseX0 = 0;
  int MouseY0 = 0;

  bool doOnce = true;

  Fluid fluid;

  sf::RenderWindow window;
  sf::Clock deltaClock;
  sf::Time deltaTime;
  sf::Font genericFont;
  sf::Text fpsText;

  sf::Texture texture;
  sf::Sprite background;
  sf::Uint8* pixels = new sf::Uint8[PIXELS];

  static const sf::Color HSV(int hue, float sat, float val, const float d) {
    hue %= 360;
    while (hue < 0) hue += 360;

    if (sat < 0.f) sat = 0.f;
    else if (sat > 1.f) sat = 1.f;

    if (val < 0.f) val = 0.f;
    else if (val > 1.f) val = 1.f;

    const int h = hue / 60;
    float f = float(hue) / 60 - h;
    float p = val * (1.f - sat);
    float q = val * (1.f - sat * f);
    float t = val * (1.f - sat * (1 - f));

    switch (h) {
      default:
      case 6: return sf::Color(val * 255, t * 255, p * 255, d);
      case 1: return sf::Color(q * 255, val * 255, p * 255, d);
      case 2: return sf::Color(p * 255, val * 255, t * 255, d);
      case 3: return sf::Color(p * 255, q * 255, val * 255, d);
      case 4: return sf::Color(t * 255, p * 255, val * 255, d);
      case 5: return sf::Color(val * 255, p * 255, q * 255, d);
    }
  }

  static const float Normalize(float val, float minIn, float maxIn, float minOut, float maxOut) {
    float x = (val - minIn) / (maxIn - minIn);
    float result = minOut + (maxOut - minOut) * x;

    return (result < minOut) ? minOut : (result > maxOut) ? maxOut : result;
  }

  void setupSFML() {
    // Setup main window
    window.create(sf::VideoMode(WIDTH, HEIGHT), "Fluid simulation", sf::Style::Close);
    window.setFramerateLimit(75);
    window.setActive(true);

    if (!ImGui::SFML::Init(window))
      throw std::runtime_error("ImGui initialize fail");

    // Font for some test text
    genericFont.loadFromFile("../../src/fonts/Minecraft rus.ttf");

    // FPS text setup
    fpsText.setString("75");
    fpsText.setFont(genericFont);
    fpsText.setCharacterSize(20);
    fpsText.setOutlineColor(sf::Color(31, 31, 31));
    fpsText.setOutlineThickness(3.f);
    fpsText.setPosition({ WIDTH - fpsText.getLocalBounds().width, 0 });

    for (int i = 0; i < PIXELS; i += 4) {
      pixels[i + 0] = 255; // Red 
      pixels[i + 1] = 255; // Green
      pixels[i + 2] = 255; // Blue
      pixels[i + 3] = 0;   // Alpha
    }

    texture.create(COLUMNS, ROWS);
    texture.update(pixels);

    background.setScale(SCALE, SCALE);
    background.setTexture(texture);
  }

  void setupProgram() {
    fluid.setup(dt, diffusion, viscocity);
  }

  void drawImGui() {
    ImGui::SFML::Update(window, deltaTime);

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

    if (ImGui::Button("Reset")) {
      densityLine = 3;
      densityValue = 100.f;

      dt = 0.1f;
      diffusion = 1.f;
      viscocity = 0.f;
    }

    ImGui::End();
  }

  void drawDensity() {
		fluid.step();

    for (int j = 0; j < COLUMNS; j++) {
      for (int i = 0; i < ROWS; i++) {
        int index = IX(j, i);
        int pixelIndex = index * 4;
        float density = fluid.getDensity(index);

        switch (fluid.getColorMode()) {
          // Black and White color mode
          case 0: {
            pixels[pixelIndex + 0] = 255;
            pixels[pixelIndex + 1] = 255;
            pixels[pixelIndex + 2] = 255;
            pixels[pixelIndex + 3] = density > 255 ? 255 : (sf::Uint8)density;
            break;
          }
          // HSV color mode
          case 1: {
            sf::Color color = HSV(density, 1.f, 1.f, 255.f);
            pixels[pixelIndex + 0] = color.r;
            pixels[pixelIndex + 1] = color.g;
            pixels[pixelIndex + 2] = color.b;
            pixels[pixelIndex + 3] = color.a;
            break;
          }
          // HSV velocity color mode
          case 2: {
            pixels[pixelIndex + 0] = (sf::Uint8)Normalize(fluid.getVelX(index), -0.05f, 0.05f, 0, 255);
            pixels[pixelIndex + 1] = (sf::Uint8)Normalize(fluid.getVelY(index), -0.05f, 0.05f, 0, 255);
            pixels[pixelIndex + 2] = 255;
            break;
          }
          default:
            break;
        };
      }
    }
    fluid.fadeDensity();

    texture.update(pixels);
    window.draw(background);
  }

  void drawOther() {
    fpsText.setString(std::to_string((int)(1.f / deltaTime.asSeconds())));
    window.draw(fpsText);
  }

  public:
    App() {}

    ~App() {
      delete[] pixels;
      ImGui::SFML::Shutdown();
    }

    void setup() {
      setupSFML();
      setupProgram();
    }

    void run() {
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

            // add density and velocity when mouse pressed
          if (event.type == sf::Event::MouseMoved) {
            MouseX = sf::Mouse::getPosition(window).x / SCALE;
            MouseY = sf::Mouse::getPosition(window).y / SCALE;

            MouseX = MouseX < 0 ? 0 : MouseX;
            MouseX = MouseX > COLUMNS - 1 ? COLUMNS - 1 : MouseX;

            MouseY = MouseY < 0 ? 0 : MouseY;
            MouseY = MouseY > ROWS - 1 ? ROWS - 1 : MouseY;

            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
              if (!ImGui::IsAnyItemHovered() &&
                  !ImGui::IsWindowHovered(ImGuiFocusedFlags_AnyWindow) &&
                  !ImGui::IsAnyItemActive()) {

                for (int i = -densityLine; i <= densityLine && MouseX + i < COLUMNS; i++) {
                  fluid.addDensity(std::abs(MouseX + i), MouseY, densityValue);
                  fluid.addVelocity(std::abs(MouseX + i), MouseY, MouseX - MouseX0, MouseY - MouseY0);
                }
              }
            }

            MouseX0 = MouseX;
            MouseY0 = MouseY;
          }
        }

        deltaTime = deltaClock.restart();

        window.clear();

        drawImGui();
        drawDensity();
        drawOther();

        ImGui::SFML::Render(window);
        window.display();
      }
    }
};

